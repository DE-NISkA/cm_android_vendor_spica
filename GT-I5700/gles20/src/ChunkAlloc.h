

#ifndef __CHUNK_ALLOC_H__
#define __CHUNK_ALLOC_H__

/*------------------------------------------------------------------------*//*!
* Handle to memory chunk
*//*-------------------------------------------------------------------------*/
class ChunkHandle
{
private:
	int g3d_fd;
	unsigned int vbase;       //!< Virtual base
	unsigned int pbase;       //!< Physical base
	unsigned int size;
	
public:
	ChunkHandle* pre;
	ChunkHandle* next;

	ChunkHandle(unsigned int size);
	~ChunkHandle();

	void*        GetPhyAddr();
	void*        GetVirtAddr();
	unsigned int GetSize();
};

//! Pointer to ChunkHandle
typedef ChunkHandle* PtrChunkH;


/*------------------------------------------------------------------------*//*!
* Chunk allocator class
* Usage: Create a ChunkAlloc with virtual base, physical base, memory size
*  and alignment and do New() and Free()
*//*-------------------------------------------------------------------------*/
class BufferManager
{
private:
	int g3d_fd;
	ChunkHandle*  chunk_list;	


public:
	BufferManager();
   ~BufferManager();

	ChunkHandle* New(unsigned int size);
	void Free(ChunkHandle* ch); 

	
	int cache_clean_invalid (void *addr, int size);
	int cache_clean         (void *addr, int size);
	int cache_invalid       (void *addr, int size);
};

/*
*******************************************************************************
* Global Variable Declarations
*******************************************************************************
*/

extern BufferManager *pCA;
/*
*******************************************************************************
* Function Declarations
*******************************************************************************
*/

#endif /* __CHUNK_ALLOC_H__ */

