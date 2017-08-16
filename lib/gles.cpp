// --------------------------------------------------------------------------

#include <stdio.h>     // fprintf, stderr
#include <stdlib.h>    // exit
#include <EGL/egl.h>   // EGL data types and functions
#include <GLES/gl.h>   // OpenGL ES 1.1 data types and functions
#include <X11/Xlib.h>  // XLib data types and functions
#include "gles.h"

// --------------------------------------------------------------------------
//   Misc
// --------------------------------------------------------------------------
// simple helpfer functions to improve readability.
// In case of value != 0 the name of the function incl. EGL error code is
// written to stderr
// More information here:
// www.khronos.org/registry/egl/sdk/docs/man/xhtml/eglGetError.html
static inline void handleEglError(const int value, const char *name)
{
	if (value == 0) { return; }
	fprintf(stderr,	"%s fehlgeschlagen!\n", name);
	fprintf(stderr, "EGL Fehler 0x%x\n", eglGetError());
	exit(-1);
}

// --------------------------------------------------------------------------
//   create OpenGL ES context incl. surface for rendering
// --------------------------------------------------------------------------
int glesInitialize(struct opengles *opengles)
{
	// ------------------------------------------------------------------
	//   XLib
	// ------------------------------------------------------------------

	// handler to connect to X-Server.
	Display *xlib_display;

	// handler to window inside X-Servers.
	Window xlib_window;

	// handler to window, where we create our own window
	Window xlib_parent;

	// attributes of window to be created
	const uint32_t  x = 0,
	                y = 0,
	            width = 1024,
	           height = 512,
	     border_width = 0,
	           border = 0,
	       background = 0;

	// ---- Initialisation ---------------------------------------------

	// connect to local X-Server (localhost) 
	xlib_display = XOpenDisplay(0);
	if (xlib_display == 0) {
		fprintf(stderr, "XOpenDisplay failed!\n");
		exit(-1);
	}

	// get handler of main window from xlib_display
	xlib_parent = XDefaultRootWindow(xlib_display);

	// create new X window
	xlib_window = XCreateSimpleWindow(xlib_display, xlib_parent,
	                                  x, y, width, height,
	                                  border_width, border, background);

	// name the window
	XStoreName(xlib_display, xlib_window, "dashboard");

	// make window visible
	XMapWindow(xlib_display, xlib_window);

	// send all commands to xserver
	XFlush(xlib_display);

	Atom wmDeleteMessage;
	wmDeleteMessage = XInternAtom(xlib_display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(xlib_display, xlib_window, &wmDeleteMessage, 1);

	// ------------------------------------------------------------------
	//   EGL
	// ------------------------------------------------------------------

	// temp variable to hold return values of functions e.g. eglInitialize.
	EGLBoolean ret;

	// two integers to hold major and minor version of EGL
	EGLint eglmajor, eglminor;

	// handler of a display, here: connection to x server
	EGLDisplay display;

	// handler of a surface, here: the frame buffer
	EGLSurface surface;

	// list of properties of the frame buffer
	const EGLint configAttr[] = {
		EGL_SURFACE_TYPE    , EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE , EGL_OPENGL_ES_BIT,
		EGL_NONE
	};

	// return values of eglChooseConfig. Contains one or more 
	// configurations of properties indicated by confgAttr
	EGLConfig   config;
	EGLint      configCount;

	// handler of a OpenGL-context
	EGLContext  context;

	// define version of the OpenGL ES context
	const EGLint contextAttr[] = {
		EGL_CONTEXT_CLIENT_VERSION , 1,
		EGL_NONE
	};

	// ---- Initialisation ---------------------------------------------

	// EGL-Display-Handler for the connection to the x server
	display = eglGetDisplay(xlib_display);
	handleEglError(display == EGL_NO_DISPLAY, "eglGetDisplay");

	// establish a connection between EGL and X-server,
	// read EGL version 
	ret = eglInitialize(display, &eglmajor, &eglminor);
	handleEglError(ret == EGL_FALSE, "eglInitialize");
	printf("EGL Version %d.%d\n", eglmajor, eglminor);

	// Defines a render API to be used by EGL, here: OpenGL ES.
	ret = eglBindAPI(EGL_OPENGL_ES_API);
	handleEglError(ret == EGL_FALSE, "eglBindAPI");

	// Check whether our display can provide a correct framebuffer
	ret = eglChooseConfig(display, configAttr, &config, 1, &configCount);
	handleEglError(ret == EGL_FALSE, "eglChooseConfig");
	if (configCount == 0)
	{
		fprintf(stderr, "eglChooseConfig failed!\n");
		fprintf(stderr, "configCount == 0, adapt configAttr!\n");
		exit(-1);
	}

	// Create a surface / framebuffer and connect with x window
	surface = eglCreateWindowSurface(display, config, xlib_window, 0);
	handleEglError(surface == EGL_NO_SURFACE, "eglCreateWindowSurface");

	// create OpenGL context
	context = eglCreateContext(display, config, 0, contextAttr);
	handleEglError(context == EGL_NO_CONTEXT, "eglCreateContext");

	// Connect display, x-window framebuffer and openGL context
	// to be used by all subsequent gl-commands
	ret = eglMakeCurrent(display, surface, surface, context);
	handleEglError(ret == EGL_FALSE, "eglMakeCurrent");

	// copy variables for OpenGLES structure
	opengles->display = display;
	opengles->surface = surface;
	opengles->context = context;
	opengles->xlib_display = xlib_display;
	opengles->xlib_window = xlib_window;
	opengles->width = (uint16_t)width;
	opengles->height = (uint16_t)height;
	opengles->ratio = (GLfloat)width / (GLfloat)height;

	// activate support for textures
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0,0, width, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glScalef((GLfloat)height / (GLfloat)width, 1.0f, 1.0f);
	//glOrthof(-1,1,-1,1,-1,1);

	// define background color after framebuffer has been cleared 
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	return 0;
}

// --------------------------------------------------------------------------
//   swap OpenGL ES Buffers
// --------------------------------------------------------------------------
int glesDraw(struct opengles *opengles)
{
	// Display the rendered image
	eglSwapBuffers(opengles->display, opengles->surface);

	return 0;
}

// --------------------------------------------------------------------------
//   OpenGL run loop
// --------------------------------------------------------------------------
int glesRun(struct opengles *gles)
{
	XEvent event;

	// process all events
	while(XPending(gles->xlib_display))
	{
		// get next event
		XNextEvent(gles->xlib_display, &event);

		// handler to catch WM_DELETE_WINDOW
		switch(event.type)
		{
		case ClientMessage: return 0; break;
		default: break;
		}
	}

	return 1;
}


// --------------------------------------------------------------------------
//   deallocate ressources
// --------------------------------------------------------------------------
int glesDestroy(struct opengles *opengles)
{
	// ---- EGL ---------------------------------------------------------

	// deactivate context
	eglMakeCurrent(opengles->display, 0, 0, 0);

	// memory for surface
	eglDestroySurface(opengles->display, opengles->surface);

	// memory for context
	eglDestroyContext(opengles->display, opengles->context);

	// close connection to display
	eglTerminate(opengles->display);

	// reset EGL backend
	eglReleaseThread();

	// ---- XLib --------------------------------------------------------

	// release window memory
	XDestroyWindow(opengles->xlib_display, opengles->xlib_window);

	// close connection to X server
	XCloseDisplay(opengles->xlib_display);

	return 0;
}


