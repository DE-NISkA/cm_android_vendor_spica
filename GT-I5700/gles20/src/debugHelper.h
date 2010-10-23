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
* \file         debugHelper.h
* \author       Sandeep Kakarlapudi (s.kakarla@samsung.com)
* \brief        Debugging helper routiens
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*       03.05.2007      Sandeep Kakarlapudi             Initial version
*
*******************************************************************************
*/

#ifndef __DEBUGHELPER_H__
#define __DEBUGHELPER_H__

#include "gl.h"
#include "platform.h"

//bool ___funcBreak(const char* funcName, const char* fileName);
void ___trackFunc(const char* funcName, const char* fileName);
int ___funcPrint(const char* format, ...);

const char* getLastGLFunc();
const char* getLastFunc();
const char* getGLErrorString(GLenum error);
const char* GLenum2Str(GLenum x);


#ifdef _DEBUG
#define ___DBG_FUNC_TRACK___    ___trackFunc(__FUNCTION__,__FILE__); 
#else
#define ___DBG_FUNC_TRACK___
#endif


#endif //__DEBUGHELPER_H__
