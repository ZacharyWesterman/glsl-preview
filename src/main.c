#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glext.h>

#include "window.h"

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		fputs("ERROR: No GLSL file specified.\n", stderr);
		return 1;
	}

	Display* display = XOpenDisplay(NULL);

	if (!display)
	{
		fputs("ERROR: Unable to connect to X server.\n", stderr);
		return 1;
	}

	int dummy;
	if (!glXQueryExtension(display, &dummy, &dummy)) {
		fputs("ERROR: OpenGL not supported by X server.\n", stderr);
		return 1;
	}

	Window window_handle = create_window(display, "glsl-preview");
	GLXContext context = create_render_context(display, window_handle);

	if (!context)
	{
		fputs("ERROR: Failed to create a GL context.\n", stderr);
		return 1;
	}

	return 0;
}
