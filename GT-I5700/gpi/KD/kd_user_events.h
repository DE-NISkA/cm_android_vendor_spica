/************************************************************************
 *                                                                      *
 *  Implementation of EGL, version 1.3                                  *
 *                                                                      *
 *  Copyright (c)2009 Samsung Inc. All Rights Reserved.                 *
 *                                                                      *
 ************************************************************************/

/************************************************************************
 *                                                                      *
 * FileName   : kd_user_events.h                                        *
 * Descrition :                                                         *
 * Notes      :                                                         *
 *                                                                      * 
 * Log                                                                  *
 *      -                                                               *
 *                                                                      *
 ************************************************************************/

#include "kd.h"

/* KD_EVENT_APPLICATION_CREATE_CONTEXT: Application create context. */
#define KD_EVENT_CREATE_CONTEXT (KD_EVENT_USER + 1)

/* kdWaitEvent: Application destroy context. */
#define KD_EVENT_DESTROY_CONTEXT (KD_EVENT_USER + 2)
