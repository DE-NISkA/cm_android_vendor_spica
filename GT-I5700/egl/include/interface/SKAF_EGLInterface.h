#ifndef __SKAF_EGLINTERFACE_H
#	define __SKAF_EGLINTERFACE_H


#ifdef __cplusplus
extern "C" {
#endif


/* SKAF style fake handles */

typedef int      TInt32;
typedef unsigned TUint32;
typedef unsigned TWIN;

typedef struct _TRect {
	TInt32 x, y, w, h;
} TRect;


/* Some test functions to create fake window */

void initialize( int* w, int* h );
void finalize( void );
TWIN create_window( int x, int y, int w, int h );
void remove_window( TWIN window );


/* SKAF interfaces already defined in SKAF */

TUint32 TWIN_GetWFimage( TWIN window, int i ); /* prototype is changed */
TInt32 TWIN_GetSize( TWIN window, TRect* rect ); /* regular api */

/* newly defined APIs for EGL */

void TWIN_SwapBuffers( TWIN window );


#ifdef __cplusplus
}
#endif


#endif /* __SKAF_EGLINTERFACE_H */
