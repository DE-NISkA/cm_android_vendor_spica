/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2008 HUONE Inc. All Rights Reserved.                   *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : egl_config.c                                            *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      *
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gles11Context.h>
#include"egl_config.h"

#define EGL_CONFIG_NUM 15

static int eglConfigList[EGL_CONFIG_NUM][33] = {
/*
    config ID,
    EGL_BUFFER_SIZE, It is the sum of EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE, and EGL_ALPHA_SIZE.
    Number of red bits stored in the color buffer.
    Number of green bits stored in the color buffer.
    Number of blue bits stored in the color buffer.
    Number of alpha bits stored in the color buffer.
    Number of luminance bits stored in the color buffer.
  color buffer type
    config caveat
    max pbuffer width
    max pbuffer height
    max pbuffer pixels
  max swap interval,
  min swap interval,
  native renderable,
  native visual ID,
  native visual type,
    renderable type,
    surface type,
  transparent type,
  transparent red value,
  transparent green value,
  transparent blue value,
    conformant,
    mached pixmap,
    alpha mask size
    level
    bind to texture RGB
    bind to texture RGBA
    depth size
    sample buffers
    samples
    stencil size
*/
  {  1, 16, 5, 6, 5, 0, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 1, 0,  0, 0, 0, 0 },
    {  2, 16, 5, 6, 5, 0, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 1, 0, 24, 0, 0, 0 },
    {  3, 16, 5, 6, 5, 0, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 1, 0, 24, 0, 0, 8 },
  {  4, 32, 8, 8, 8, 8, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 0, 1,  0, 0, 0, 0 },
    {  5, 32, 8, 8, 8, 8, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 0, 1, 24, 0, 0, 0 },
    {  6, 32, 8, 8, 8, 8, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 0, 1, 24, 0, 0, 8 },
  {  7, 32, 8, 8, 8, 0, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 0, 1,  0, 0, 0, 0 },
  {  8, 32, 8, 8, 8, 0, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 0, 1, 24, 0, 0, 0 },
    {  9, 32, 8, 8, 8, 0, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 0, 1, 24, 0, 0, 8 },
  { 10, 16, 4, 4, 4, 4, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 1, 0,  0, 0, 0, 0 },
    { 11, 16, 4, 4, 4, 4, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 1, 0, 24, 0, 0, 0 },
    { 12, 16, 4, 4, 4, 4, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 1, 0, 24, 0, 0, 8 },
    { 13, 16, 5, 5, 5, 1, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 1, 0,  0, 0, 0, 0 },
    { 14, 16, 5, 5, 5, 1, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 1, 0, 24, 0, 0, 0 },
    { 15, 16, 5, 5, 5, 1, 0, EGL_RGB_BUFFER, EGL_NONE, 4096, 4096, 16777216, 0, 0, EGL_FALSE, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT | EGL_LOCK_SURFACE_BIT_KHR, EGL_NONE, 0, 0, 0, EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT, EGL_NONE, 0, 0, 1, 0, 24, 0, 0, 8 },
        };


EGLint eglGetConfigNum()
{
    return EGL_CONFIG_NUM;
}

EGLint (*eglGetConfigList())[33]
{
    return eglConfigList;
}
