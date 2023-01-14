#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "shader.h"

GLuint Program;
struct timespec u_time;

void draw(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);

	GLfloat elapsed = (now.tv_sec + now.tv_nsec * 1e-9) - (u_time.tv_sec - u_time.tv_nsec * 1e-9);

	glUniform1f(glGetUniformLocation(Program, "u_time"), elapsed);

	glUseProgram(Program);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
	glutSwapBuffers();
}

void resize(int w, int h)
{
	glUniform2f(glGetUniformLocation(Program, "u_resolution"), w, h);
	glViewport(0, 0, w, h);
}

void draw_timer(int id)
{
	draw();
	glutTimerFunc(1000 / 60, draw_timer, 0);
}

int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(700, 700);
	glutCreateWindow("glsl-preview");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		fputs("ERROR: Failed to init GLEW.\n", stderr);
		exit(EXIT_FAILURE);
	}

	Program = (argc < 2) ? shader("cfg/default_fragment.glsl") : shader(argv[1]);

	// glutDisplayFunc(draw);
	glutReshapeFunc(resize);
	glutTimerFunc(1000 / 60, draw_timer, 0);

	clock_gettime(CLOCK_REALTIME, &u_time);
	glutMainLoop();

	return 0;
}
