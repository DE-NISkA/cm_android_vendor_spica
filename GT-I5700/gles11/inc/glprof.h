/*
*******************************************************************************
*
*                        SAMSUNG INDIA SOFTWARE OPERATIONS
*                               Copyright(C) 2006
*                              ALL RIGHTS RESERVED
*
* This program is proprietary  to Samsung India  Software Operations Pvt. Ltd.,
* and is protected under International Copyright Act as an unpublished work.Its
* use and disclosure is limited by the terms and conditions of a license agree-
* -ment. It may not be  copied or otherwise  reproduced or disclosed to persons
* outside the licensee's  organization except in accordance  with the terms and
* conditions of  such an agreement. All copies and  reproductions  shall be the 
* property of  Samsung  India Software Operations Pvt. Ltd.  and must bear this
* notice in its entirety.
*
*******************************************************************************
*/

/*
***************************************************************************//*!
*
* \file         glProf.h
* \author       Sandeep Kakarlapudi (s.kakarla@samsung.com)
* \brief        Interface for the glprof profiler
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*

*   20.09.2007  Sandeep Kakarlapudi     Added report printing, non GPU samples
*       06.09.2007      Sandeep Kakarlapudi             Initial version
*
*******************************************************************************
*/
#ifndef __GLPROF_H__
#define __GLPROF_H__

enum { GLPROF_GL = 1, GLPROF_GPU = 2};
enum GLProfStart {GLPROF_NOW, GLPROF_ON_BUFFER_SWAP};

struct glprof_glstats {
    float fps;              //frames per second
    float primsPerSec;      //primitives per second
    float drawCallsPerSec;  //draw calls per second - not really interesting 
                            //  for most demos
    float timeinterval;     //time interval over which these stats have been
                            // gathered in secs. User can use this to get 
                            // total number of frames / prims etc.
                            // eg. total frames = fps*timeinterval;
};

bool glprofInit(unsigned int profFlags);
bool glprofDeinit();
bool glprofReset();
void glprofStart(GLProfStart start = GLPROF_ON_BUFFER_SWAP);
void glprofStop();

//glprofStop must be called before the following functions
void glprofPrintReport();
glprof_glstats glprofGetGlStats();

//internally used functions
void glprofBufferSwap();
void glprofPrimitives(unsigned int glprimType, unsigned int n);

#endif //__GLPROF_H__
