
#ifndef _PROFILER_H_
#define _PROFILER_H_
#include <time.h>

clock_t startTime;

typedef struct 
{
        clock_t startTime;                                      // Start time of the timer

        float timeDiffinSec;                            // Time difference between start and end time

}Timer;

void getStartTime (Timer *t);
void getTimeDiff (Timer *t);


#endif //_PROFILER_H_