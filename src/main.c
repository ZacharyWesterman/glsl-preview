#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "shader.h"

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		fputs("ERROR: No GLSL file specified.\n", stderr);
		exit(EXIT_FAILURE);
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutCreateWindow("glsl-preview");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		fputs("ERROR: Failed to init GLEW.\n", stderr);
		exit(EXIT_FAILURE);
	}

	glEnable(GL_DEPTH_TEST);
	GLuint program = shader(argv[1]);
	glActiveTexture(GL_TEXTURE0);

	glutMainLoop();

	return 0;
}
