#ifndef __DMA_H_
#define __DMA_H_

#include <stdio.h>
#include <stdlib.h> /* for exit */
#include <fcntl.h> /* for O_RDWR */

#ifdef linux
#include <unistd.h> /* for open/close .. */
#include <sys/ioctl.h> /* for ioctl */
#include <sys/mman.h>
#endif

#define MAX_BUFFER_TABLE 1000

#define DMA_MODE 1

typedef struct{
        unsigned int offset; // should be word aligned

        unsigned int size; // byte size, should be word aligned

}DMA_BLOCK_STRUCT;

typedef struct
{
        int *VirtualAddress;
        int *PhysicalAddress;
        int size;
}DMA_BUFFER_TABLE;

int* GetDMABuffer(int size);
void  ReleaseDMABuffer(int* VirtualAddress);
void StartDMAOperation(int *VirtualAddress,int size);
int InitDMATable(int *VirtualBufferAddr, int *PhysicalBufferAddr, int size,int handle);
int IsDMABlock(int *VirtualAddress);

#endif //__S3CTVSCALER_H_
