#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <GL/glew.h>
// #include <GL/glut.h>
#include <GL/freeglut.h>

#include "shader.h"

char shader_path[4096] = "\0";
char shader_error[65536] = "\0";
int active_program = 0;
GLuint Program;
struct timespec u_time;
GLuint width, height;

void draw(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (active_program)
	{
		struct timespec now;
		clock_gettime(CLOCK_REALTIME, &now);

		GLfloat elapsed = (now.tv_sec + now.tv_nsec * 1e-9) - (u_time.tv_sec - u_time.tv_nsec * 1e-9);

		glUniform1f(glGetUniformLocation(Program, "u_time"), elapsed);
		glUseProgram(Program);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
	}

	if (shader_error[0])
	{
		float x = -1.0 + 20.0 / (float)width;
		float y = 1.0 - 50.0 / (float)height;
		glRasterPos2f(x, y);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, shader_error);
	}

	glutSwapBuffers();
}

void resize(int w, int h)
{
	width = w; height = h;
	if (active_program) {
		glUniform2f(glGetUniformLocation(Program, "u_resolution"), w, h);
	}
	glViewport(0, 0, w, h);
}

void mouse(int x, int y)
{
	if (active_program) {
		glUniform2f(glGetUniformLocation(Program, "u_mouse"), x / (float)width, 1.0 - y / (float)height);
	}
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_SRC_ALPHA);

	strcpy(shader_path, (argc < 2) ? "cfg/default_fragment.glsl" : argv[1]);
	Program = shader(shader_path, &active_program, shader_error);

	glutDisplayFunc(draw);
	glutReshapeFunc(resize);
	glutMotionFunc(mouse);
	glutPassiveMotionFunc(mouse);
	glutTimerFunc(1000 / 60, draw_timer, 0);

	clock_gettime(CLOCK_REALTIME, &u_time);
	glutMainLoop();

	return 0;
}
