#include <unistd.h>
#include <stdio.h>

#ifdef __DEBUG
#define _DEBUG printf
#else
#define _DEBUG (void)
#endif

void letsleep(unsigned int);

void letsleep(unsigned int usec)
{
        _DEBUG("let sleep %u",usec);    
        usleep(usec);
}
