/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : sec_gles20.h                                            *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#ifndef EGL_GLES20_SEC_ADAPTER_H
#define EGL_GLES20_SEC_ADAPTER_H

#include <EGL/egl.h>

void GLESSetColorBuffer20(iEGLSurface *surface, EGLint readdraw);

void* GLESCreateContext20(EGLContext share_context);

EGLBoolean GLESSetContext20(void* context);

void GLESDestroyContext20(void* clientContext);

void GLESBindTexImage20(iEGLSurface *surface);

void GLESReleaseTexImage20(iEGLSurface *surface);

void GLESFlush20();

void GLESFinish20();

void GLESViewport20(EGLint x, EGLint y, EGLint width, EGLint height);

void GLESScissor20(EGLint x, EGLint y, EGLint width, EGLint height);

void GLESDeinitDriver20();

#endif // EGL_GLES20_SEC_ADAPTER_H
