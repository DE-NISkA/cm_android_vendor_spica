/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : sec_gles11.h                                            *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#ifndef EGL_GLES11_SEC_ADAPTER_H
#define EGL_GLES11_SEC_ADAPTER_H

#include <EGL/egl.h>

void GLESSetColorBuffer11(iEGLSurface *surface, EGLint readdraw);

void* GLESCreateContext11(EGLContext share_context);

EGLBoolean GLESSetContext11(void* context);

void GLESDestroyContext11(void* clientContext);

void GLESBindTexImage11(iEGLSurface *surface);

void GLESReleaseTexImage11(iEGLSurface *surface);

void GLESFlush11();

void GLESFinish11();

void GLESViewport11(EGLint x, EGLint y, EGLint width, EGLint height);

void GLESScissor11(EGLint x, EGLint y, EGLint width, EGLint height);

void GLESDeinitDriver11();

#endif // EGL_GLES11_SEC_ADAPTER_H
