/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2009 Samsung Inc. All Rights Reserved.                 *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : GPI_shared_libraries_linux.c                            *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      * 
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#include <dlfcn.h>

#include <GPI/gpi.h>

#if 0
/* GPI_LoadLibrary: load a shared library */
GPI_Handle GPI_LoadLibrary(const char* name)
{
//    return dlopen(name, RTLD_LAZY);
}

/* GPI_LoadLibrary: obtaining the address of a symbol from given shared library */
GPI_Handle GPI_GetProcAddress(GPI_Handle handle, const char* name)
{
//    return dlsym(handle, name);
}

/* GPI_LoadLibrary: freeing a library loaded by GPI_LoadLibrary */
GPI_Error GPI_FreeLibrary(GPI_Handle handle)
{
//    return dlclose(handle);
}
#endif

