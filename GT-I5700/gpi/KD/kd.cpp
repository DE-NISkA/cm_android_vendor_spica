/**
 * Samsung Project
 * Copyright (c) 2009 Mobile Solution, Samsung Electronics, Inc.
 * All right reserved.
 *
 * This software is the confidential and proprietary information
 * of Samsung Electronics Inc. ("Confidential Information"). You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung Electronics.
 */

#include <pthread.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include <KD/kd.h>
#include <KD/KHR_formatted.h>

#include <cutils/log.h>


/**************************** Static variables *************************/
static pthread_key_t g_TLSKey = 0;
/***********************************************************************/


/* kdThreadOnce: Wrap initialization code so it is executed only once. */
KD_API KDint KD_APIENTRY kdThreadOnce(KDThreadOnce *once_control, void (*init_routine)(void))
{
    kdAssert(once_control != 0 && init_routine != 0);

        return pthread_once((pthread_once_t*)once_control, init_routine);
}

/* kdMalloc: Allocate memory. */
KD_API void* KD_APIENTRY kdMalloc(KDsize size)
{
        return malloc(size);
}

/* kdFree: Free allocated memory block. */
KD_API void KD_APIENTRY kdFree(void* ptr)
{
        free(ptr);
}

/* kdMemcpy: Copy a memory region, no overlapping. */
KD_API void* KD_APIENTRY kdMemcpy(void* buf, const void* src, KDsize len)
{
    kdAssert(len == 0 || (buf != 0 && src != 0));

        return memcpy(buf, src, len);
}

/* kdMemset: Set bytes in memory to a value. */
KD_API void* KD_APIENTRY kdMemset(void* buf, KDint byte, KDsize len)
{
    kdAssert(len == 0 || buf != 0);

        return memset(buf, byte, len);
}

/* kdMemcmp: Compare two memory regions. */
KD_API KDint KD_APIENTRY kdMemcmp(const void *src1, const void *src2, KDsize len)
{
    kdAssert(len == 0 || (src1 != 0 && src2 !=0));

        return memcmp(src1, src2, len);
}

/* kdStrcpy_s: Copy a string with an overrun check. */
KD_API KDint KD_APIENTRY kdStrcpy_s(KDchar *buf, KDsize buflen, const KDchar *src)
{
        kdAssert(buflen == 0 || (buf != 0 && src != 0));
        strcpy(buf, src);
        return 0;
}

/* kdStrncpy_s: Copy a string with an overrun check. */
KD_API KDint KD_APIENTRY kdStrncpy_s(KDchar *buf, KDsize buflen, const KDchar *src, KDsize srclen)
{
        kdAssert(buflen == 0 || (buf != 0 && src != 0));
        strncpy(buf, src, srclen);
        return 0;
}

/* kdStrlen: Determine the length of a string. */
KD_API KDsize KD_APIENTRY kdStrlen(const KDchar *str)
{
    kdAssert(str != 0);
    
    return strlen(str);
}

/* kdStrcmp: Compares two strings. */
KD_API KDint KD_APIENTRY kdStrcmp(const KDchar *str1, const KDchar *str2)
{
        kdAssert(str1 != 0 && str1 != 0);
        return strcmp(str1, str2);
}

/* kdStrncmp: Compares two strings with length limit. */
KD_API KDint KD_APIENTRY kdStrncmp(const KDchar* str1, const KDchar* str2, KDsize maxlen)
{
    kdAssert(maxlen == 0 || (str1 != 0 && str1 != 0));

        return strncmp(str1, str2, maxlen);
}

/* kdThreadMutexCreate: Create a mutex. */
KD_API KDThreadMutex* KD_APIENTRY kdThreadMutexCreate(const void *mutexattr)
{
        pthread_mutex_t* mutex;

        int ret;
        pthread_mutexattr_t attr;

        ret = pthread_mutexattr_init(&attr);
        if (ret != 0)
        {
                return 0;
        }

        // create reentrant mutex by default
        ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
        if (ret != 0)
        {
                pthread_mutexattr_destroy(&attr);
                return 0;
        }

        mutex = (pthread_mutex_t*)kdMalloc(sizeof(pthread_mutex_t));
        if (mutex == 0)
        {
                pthread_mutexattr_destroy(&attr);
                return 0;
        }

        ret = pthread_mutex_init(mutex, &attr);
        if (ret != 0)
        {
                pthread_mutexattr_destroy(&attr);
                kdFree(mutex);
                return 0;
        }

        return (KDThreadMutex*)mutex;
}

/* kdThreadMutexFree: Free a mutex. */
KD_API KDint KD_APIENTRY kdThreadMutexFree(KDThreadMutex *mutex)
{
    kdAssert(mutex != 0);

        int ret = pthread_mutex_destroy((pthread_mutex_t*)mutex);

        kdFree(mutex);

        return ret;
}

/* kdThreadMutexLock: Lock a mutex. */
KD_API KDint KD_APIENTRY kdThreadMutexLock(KDThreadMutex *mutex)
{
    kdAssert(mutex != 0);

        return pthread_mutex_lock((pthread_mutex_t*)mutex);
}

/* kdThreadMutexUnlock: Unlock a mutex. */
KD_API KDint KD_APIENTRY kdThreadMutexUnlock(KDThreadMutex *mutex)
{
    kdAssert(mutex != 0);

        return pthread_mutex_unlock((pthread_mutex_t*)mutex);
}

/* kdThreadSelf: Return calling thread's ID. */
KD_API KDThread* KD_APIENTRY kdThreadSelf(void)
{
        return (KDThread*)pthread_self();
}

/* kdHandleAssertion: Handle assertion failure. */
KD_API void KD_APIENTRY kdHandleAssertion(const KDchar *condition, const KDchar *filename, KDint linenumber)
{
    kdAssert(condition != 0 && filename != 0);

//    __assert(condition, filename, linenumber);
}

/* kdLogMessage: Output a log message. */
KD_API void KD_APIENTRY kdLogMessage(const KDchar* string)
{
    kdAssert(string != 0);

        LOGE(string);
}


KD_API KDint KD_APIENTRY kdVsnprintfKHR(KDchar *buf, KDsize bufsize, const KDchar *format, KDVaListKHR ap)
{
    vsnprintf(buf, bufsize, format, ap);
    return 0;
}

KD_API KDint KD_APIENTRY kdVsprintfKHR(KDchar *buf, const KDchar *format, KDVaListKHR ap)
{
        vsprintf(buf, format, ap);
        return 0;
}

/* kdLogMessagefKHR: Formatted output to the platform's debug logging facility. */

KD_API KDint KD_APIENTRY kdLogMessagefKHR(const KDchar *format, ...)
{
    KDchar buffer[4096];

        va_list ap;
 
    va_start(ap, format);
    kdVsprintfKHR(buffer, format, ap);
    va_end(ap);

    kdLogMessage(buffer);
    return 0;
}

/* kdCreateEvent: Create an event for posting. */
KD_API KDEvent *KD_APIENTRY kdCreateEvent(void)
{
    return (KDEvent*)kdMalloc(sizeof(KDEvent));
}

/* kdFreeEvent: Abandon an event instead of posting it. */
KD_API void KD_APIENTRY kdFreeEvent(KDEvent *event)
{
    kdFree(event);
}

/* kdPostEvent: Post an event into a queue. */
KD_API KDint KD_APIENTRY kdPostEvent(KDEvent *event)
{
    kdFree(event);
    return 0;
}

/* kdGetTLS: Get the thread-local storage pointer. */
KD_API void *KD_APIENTRY kdGetTLS(void)
{
        return pthread_getspecific(g_TLSKey);
}

/* kdSetTLS: Set the thread-local storage pointer. */
KD_API void KD_APIENTRY kdSetTLS(void *ptr)
{
        if(g_TLSKey == 0)
        {
                pthread_key_create(&g_TLSKey, NULL);
        }

        pthread_setspecific(g_TLSKey, ptr);
}

/*******************************************************
 * Time functions
 *******************************************************/

/* kdGetTimeUST: Get the current unadjusted system time. */
KD_API KDust KD_APIENTRY kdGetTimeUST(void)
{
        struct timeval curr;
        KDust usec;

        gettimeofday(&curr, 0);

        usec = curr.tv_sec * 1000000 + curr.tv_usec;

        return usec;
}


