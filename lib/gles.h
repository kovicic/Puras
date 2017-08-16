// --------------------------------------------------------------------------


// include header only once
#ifndef GLES_H_
#define GLES_H_ 1

#include <EGL/egl.h>   // EGL data types and functions
#include <GLES/gl.h>   // OpenGL ES 1.1 data types and functions
#include <X11/Xlib.h>  // XLib data types and functions

struct opengles
{
	// EGL
	EGLDisplay  display;
	EGLSurface  surface;
	EGLContext  context;

	// Xlib
	Display    *xlib_display;
	Window      xlib_window;

	// Windows Dimensions
	uint16_t    width;
	uint16_t    height;
	float       ratio;
};

// function prototypes
int glesInitialize(struct opengles *opengles);
int glesDraw(struct opengles *opengles);
int glesRun(struct opengles *gles);
int glesDestroy(struct opengles *opengles);

#endif
