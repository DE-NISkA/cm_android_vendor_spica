
/*
*******************************************************************************
* Includes
*******************************************************************************
*/
#include <stdio.h>
#include "ChunkAlloc.h"
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/android_pmem.h>
#include <cutils/log.h>

/*
*******************************************************************************
* Type, Structure & Class Definitions
*******************************************************************************
*/

ChunkHandle::ChunkHandle(unsigned int _size)
{
	struct pmem_region region;
	g3d_fd = vbase = pbase = size = 0;

	pre=NULL;
	next=NULL;

//	LOGE("BM: alloc new chunk=%d",_size);

	_size = (_size+31 & 0xffffffe0); // 32 bytes alignment
	
	g3d_fd=open("/dev/pmem_gpu1", O_RDWR | O_SYNC);
	
	if(g3d_fd<0) {
		LOGE("BM: open err /dev/pmem_gpu1");
		return;
	}
//	else LOGE("BM: /dev/pmem_gpu1 was opened successfully!");

	vbase = (unsigned int) mmap(0, _size, PROT_READ|PROT_WRITE, MAP_SHARED, g3d_fd, 0);
	
	if(!vbase)
	{
		LOGE("BM: mmap err");
		return;
	}

	if (ioctl(g3d_fd, PMEM_GET_PHYS, &region)>=0)
		pbase = region.offset;
	else
	{
		LOGE("BM: get PMEM_GET_PHYS err");
		return;
	}

	size=_size;
//	LOGE("BM: new Chunk size=%d padd=0x%x vaddr=0x%x",size,pbase,vbase);
}

/*------------------------------------------------------------------------*//*!
* Get chunk's physical address
*//*-------------------------------------------------------------------------*/
void*
ChunkHandle::GetPhyAddr()
{
	return (void*)(pbase);
}

/*------------------------------------------------------------------------*//*!
* Get chunk's virtual address
*//*-------------------------------------------------------------------------*/
void*
ChunkHandle::GetVirtAddr()
{
	return (void*)(vbase);
}

/*------------------------------------------------------------------------*//*!
* Get chunk's size
*//*-------------------------------------------------------------------------*/
unsigned int
ChunkHandle::GetSize()
{
	return size;
}

/*------------------------------------------------------------------------*//*!
* chunk destructor
*//*-------------------------------------------------------------------------*/
ChunkHandle::~ChunkHandle()
{
//	LOGE("BM: trying unmap chunk");
	if(munmap((void*)vbase,size)<0)
	{
		LOGE("BM: unmap err size=%d",size);
	}
//	else LOGE("BM: unmap size=%d",size);
	close(g3d_fd);
}


/*------------------------------------------------------------------------*//*!
* BufferManager class
*//*-------------------------------------------------------------------------*/
#ifdef CACHE_MEM
int BufferManager::cache_invalid(void *addr, int size)
{
	struct pmem_mem_alloc alloc_desc;
	alloc_desc.size = size;
	alloc_desc.vir_addr = (unsigned int)addr;
	alloc_desc.phy_addr = 0;

	if( ioctl(g3d_fd, PMEM_CACHE_INVALID, &alloc_desc) ) {
		LOGE("BufferManager unable to invaild cache addr = 0x%x, size = %d\n", (unsigned int)addr, size);
		return -1;
	}

return 0;
}

int BufferManager::cache_clean(void *addr, int size)
{
	struct pmem_mem_alloc alloc_desc;
	alloc_desc.size = size;
	alloc_desc.vir_addr = (unsigned int)addr;
	alloc_desc.phy_addr = 0;

	if( ioctl(g3d_fd, PMEM_CACHE_CLEAN, &alloc_desc) ) {
		LOGE("BufferManager unable to clean cache addr = 0x%x, size = %d\n", (unsigned int)addr, size);
		return -1;
	}

return 0;
}

int BufferManager::cache_clean_invalid(void *addr, int size)
{
	struct pmem_mem_alloc alloc_desc;
	alloc_desc.size = size;
	alloc_desc.vir_addr = (unsigned int)addr;
	alloc_desc.phy_addr = 0;

	if( ioctl(g3d_fd, PMEM_CACHE_CLEAN_INVALID, &alloc_desc) ) {
		LOGE("BufferManager unable to clean and invalid cache addr = 0x%x, size = %d\n", (unsigned int)addr, size);
		return -1;
	}

return 0;
}
#endif
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
BufferManager::BufferManager()
{
	g3d_fd = 0;
	chunk_list = NULL;

	LOGE("BufferManager INIT\n");
	g3d_fd=open("/dev/pmem_gpu1", O_RDWR );
	
	if(g3d_fd<0)
	LOGE("BM: open err /dev/pmem_gpu1");
//	else LOGE("BM: /dev/pmem_gpu1 was opened successfully!");

}
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
BufferManager::~BufferManager()
{
	ChunkHandle* delete_chunk;

	LOGE("BM: garbage collect start");
	while(chunk_list)
	{
		delete_chunk = chunk_list;
		chunk_list   = delete_chunk->pre;
		delete delete_chunk;
	}
	LOGE("BM: garbage collect end");

	close(g3d_fd);
}
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
ChunkHandle* BufferManager::New(unsigned int size)
{
	if(size == 0) {
		LOGE("BufferManager received an alloc request of size 0, 4 bytes will be alloced\n");
		size = 4;
	}

	ChunkHandle* res = new ChunkHandle(size);
	res->next  = NULL;
	res->pre   = chunk_list;
	
	if(chunk_list)
	chunk_list->next = res;
	
	chunk_list = res;

	return res;
}

void BufferManager::Free(ChunkHandle* ch)
{
	if(ch == NULL)
	{
		LOGE("BM: cant free chunk - NULL pointer");
		return;
	}

	if(ch->next == NULL)
	chunk_list = ch->pre;
	
	if(ch->pre)
	ch->pre->next = ch->next;
	
	if(ch->next)
	ch->next->pre = ch->pre;
	
	delete ch;
}
