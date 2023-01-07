#include "window.h"
#include <stdio.h>
#include <string.h>

Bool WaitForMapNotify(Display *d, XEvent *e, char *arg)
{
	return d && e && arg && (e->type == MapNotify) && (e->xmap.window == *(Window*)arg);
}

GLXFBConfig get_fbconfig(Display* display)
{
	const int VisData[] = {
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_DOUBLEBUFFER, True,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 16,
		None
	};
	int screen = DefaultScreen(display);
	int numfbconfigs;

	GLXFBConfig* fbconfigs = glXChooseFBConfig(display, screen, VisData, &numfbconfigs);

	GLXFBConfig fbconfig = 0;
	for(int i = 0; i<numfbconfigs; i++) {
		XVisualInfo* visual = (XVisualInfo*) glXGetVisualFromFBConfig(display, fbconfigs[i]);
		if(!visual)
			continue;

		XRenderPictFormat* pict_format = XRenderFindVisualFormat(display, visual->visual);
		if(!pict_format)
			continue;

		fbconfig = fbconfigs[i];
		if(pict_format->direct.alphaMask > 0) {
			break;
		}
		XFree(visual);
	}

	return fbconfig;
}

Window create_window(Display* display, char* restrict title)
{
	int screen = DefaultScreen(display);
	Window root = RootWindow(display, screen);

	Window window_handle = XCreateSimpleWindow(
		display,
		root,
		0, 0, //x, y
		640, 480, //width, height
		0, //border width
		0, //border
		0 //background
	);

	XTextProperty textprop = {
		title,
		XA_STRING, //encoding
		8, //format
		strlen(title) //text length
	};

	XSizeHints hints = {
		0, 0, //x, y
		640, 480, //width, height
		USPosition | USSize //flags
	};

	XWMHints* startup_state = XAllocWMHints();
	startup_state->initial_state = NormalState;
	startup_state->flags = StateHint;

	XSetWMProperties(
		display,
		window_handle,
		&textprop,
		&textprop,
		NULL,
		0,
		&hints,
		startup_state,
		NULL
	);
	XFree(startup_state);

	XMapWindow(display, window_handle);

	XEvent event;
	XIfEvent(display, &event, WaitForMapNotify, (char*)&window_handle);

	Atom del_atom = XInternAtom(display, "WM_DELETE_WINDOW", 0);
	if (del_atom != None)
	{
		XSetWMProtocols(display, window_handle, &del_atom, 1);
	}

	return window_handle;
}

GLXContext create_render_context(Display* display, Window window_handle)
{
	GLXFBConfig fbconfig = get_fbconfig(display);
	GLXContext render_context = glXCreateNewContext(display, fbconfig, GLX_RGBA_TYPE, 0, True);
	if (render_context)
	{
		if (!glXMakeContextCurrent(display, window_handle, window_handle, render_context))
		{
			return NULL;
		}
	}

	return render_context;
}
