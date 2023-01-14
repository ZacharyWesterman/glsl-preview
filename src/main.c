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

#define elapse_bufsiz 40
float frame_elapsed[elapse_bufsiz] = {0.0f};
int elapse_counter = 0;
float prev_frame = 0.0;

float get_fps(void)
{
	float sum = 0.0f;
	for (int i=0; i< elapse_bufsiz; ++i)
	{
		sum += frame_elapsed[i];
	}

	return (float)elapse_bufsiz / sum;
}

float track_fps(float now)
{
	if (elapse_counter >= elapse_bufsiz) elapse_counter = 0;
	frame_elapsed[elapse_counter++] = now - prev_frame;
	prev_frame = now;
}

void draw(void)
{
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	GLfloat elapsed = (now.tv_sec + now.tv_nsec * 1e-9) - (u_time.tv_sec - u_time.tv_nsec * 1e-9);
	track_fps(elapsed);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (active_program)
	{
		glUniform2f(glGetUniformLocation(Program, "u_resolution"), width, height);
		glUniform1f(glGetUniformLocation(Program, "u_time"), elapsed);
		glUseProgram(Program);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
	}

	if (shader_error[0])
	{
		glWindowPos2i(10, height - 20);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, shader_error);
	}
	else
	{
		char frames[100];
		sprintf(frames, "glsl-preview [%.0f fps]", get_fps());
		glutSetWindowTitle(frames);
	}

	glutSwapBuffers();
}

void resize(int w, int h)
{
	width = w; height = h;
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

void select_file(int id)
{
	shader_path[0] = '\0';
	FILE* fp = popen("zenity --file-selection --file-filter=\"OpenGL shader source | *.glsl\" --file-filter=\"All files | *\"", "r");
	fgets(shader_path, 4096, fp);

	if (!shader_path[0]) return;
	shader_path[strlen(shader_path)-1] = '\0'; //remove newline from the end.

	Program = shader(shader_path, &active_program, shader_error, 0);
}

void menu(int action)
{
	glutTimerFunc(100, select_file, 0);
}

int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(700, 700);
	glutCreateWindow("glsl-preview");

	//Basic "load file" menu. don't need anything else.
	glutCreateMenu(menu);
	glutAddMenuEntry("Load New Shader", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		fputs("ERROR: Failed to init GLEW.\n", stderr);
		exit(EXIT_FAILURE);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_SRC_ALPHA);

	const char defaultfrag[] =
	"uniform float u_time;\n"
	"void main()\n"
	"{\n"
	"	float brightness = (sin(u_time) + 2.0) / 5.0;\n"
	"	gl_FragColor = vec4(brightness);\n"
	"}\n";

	strcpy(shader_path, (argc < 2) ? defaultfrag : argv[1]);
	Program = shader(shader_path, &active_program, shader_error, (argc < 2));

	glutDisplayFunc(draw);
	glutReshapeFunc(resize);
	glutMotionFunc(mouse);
	glutPassiveMotionFunc(mouse);
	glutTimerFunc(1000 / 60, draw_timer, 0);

	clock_gettime(CLOCK_REALTIME, &u_time);
	glutMainLoop();

	return 0;
}
