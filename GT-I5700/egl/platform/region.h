/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2009 Samsung Inc. All Rights Reserved.                 *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : region.h                                                *
 * Descrition : cpp interface between EGL and Eclair                    *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#ifndef EGL_ECLAIR_REGION_H
#define EGL_ECLAIR_REGION_H

inline int min(int a, int b)
{
    return (a<b) ? a : b;
}

inline int max(int a, int b)
{
    return (a>b) ? a : b;
}

struct Rect
{
    inline Rect() { };
    inline Rect(int32_t w, int32_t h)
        : left(0), top(0), right(w), bottom(h) { }
    inline Rect(int32_t l, int32_t t, int32_t r, int32_t b)
        : left(l), top(t), right(r), bottom(b) { }
    Rect& andSelf(const Rect& r) {
        left   = max(left, r.left);
        top    = max(top, r.top);
        right  = min(right, r.right);
        bottom = min(bottom, r.bottom);
        return *this;
    }
    bool isEmpty() const {
        return (left>=right || top>=bottom);
    }

    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
};

struct Region
{
    inline Region() : count(0) { }
    typedef Rect const* const_iterator;
    const_iterator begin() const { return storage; }
    const_iterator end() const { return storage+count; }
    static Region subtract(const Rect& lhs, const Rect& rhs) {
        Region reg;
        Rect* storage = reg.storage;
        if (!lhs.isEmpty()) {
            if (lhs.top < rhs.top) { // top rect
                storage->left   = lhs.left;
                storage->top    = lhs.top;
                storage->right  = lhs.right;
                storage->bottom = rhs.top;
                storage++;
            }
            const int32_t top = max(lhs.top, rhs.top);
            const int32_t bot = min(lhs.bottom, rhs.bottom);
            if (top < bot) {
                if (lhs.left < rhs.left) { // left-side rect
                    storage->left   = lhs.left;
                    storage->top    = top;
                    storage->right  = rhs.left;
                    storage->bottom = bot;
                    storage++;
                }
                if (lhs.right > rhs.right) { // right-side rect
                    storage->left   = rhs.right;
                    storage->top    = top;
                    storage->right  = lhs.right;
                    storage->bottom = bot;
                    storage++;
                }
            }
            if (lhs.bottom > rhs.bottom) { // bottom rect
                storage->left   = lhs.left;
                storage->top    = rhs.bottom;
                storage->right  = lhs.right;
                storage->bottom = lhs.bottom;
                storage++;
            }
            reg.count = storage - reg.storage;
        }
        return reg;
    }
    bool isEmpty() const {
        return count<=0;
    }
private:
    Rect storage[4];
    ssize_t count;
};

inline void copyBlt(void* dst_vaddr,void const* src_vaddr, const Region& clip, int dstStride, int srcStride, int bpp)
{
    Region::const_iterator cur = clip.begin();
    Region::const_iterator end = clip.end();

    const size_t dbpr = dstStride * bpp;
    const size_t sbpr = srcStride * bpp;

    unsigned char const * const src_bits = (unsigned char const *)src_vaddr;
    unsigned char *const dst_bits = (unsigned char*)dst_vaddr;

    while (cur != end) {
        const Rect& r(*cur++);
        ssize_t w = r.right - r.left;
        ssize_t h = r.bottom - r.top;
        if (w <= 0 || h<=0) continue;
        size_t size = w * bpp;
        unsigned char const * s = src_bits + (r.left + srcStride * r.top) * bpp;
        unsigned char* d = dst_bits + (r.left + dstStride * r.top) * bpp;
        if (dbpr==sbpr && size==sbpr) {
            size *= h;
            h = 1;
        }
        do {
            memcpy(d, s, size);
            d += dbpr;
            s += sbpr;
        } while (--h > 0);
    }
}

#endif // EGL_ECLAIR_REGION_H
