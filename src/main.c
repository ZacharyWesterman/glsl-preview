#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>

GLuint program;

char* load_shader(const char* path)
{
	FILE* fp = fopen(path, "r");
	char* buf;
	long size;

	if (fp == NULL) return NULL;

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	buf = malloc(size + 1);
	fread(buf, 1, size, fp);
	buf[size] = '\0';
	fclose(fp);

	return buf;
}

void shader(const char* path)
{
	const char* shader_source = load_shader(path);
	if (!shader_source)
	{
		fputs("ERROR: Failed to load file.\n", stderr);
		exit(EXIT_FAILURE);
	}

	//Create shader and attach it to program.
	program = glCreateProgram();
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glAttachShader(program, fragment_shader);

	//Compile the shader
	glShaderSource(fragment_shader, 1, &shader_source, NULL);
	glCompileShader(fragment_shader);

	GLint status = 0;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length = 0;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &length);
		char log[length];
		glGetShaderInfoLog(fragment_shader, length, &length, log);

		fputs(log, stderr);
		exit(EXIT_FAILURE);
	}

	//link and error check
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char log[length];
		glGetProgramInfoLog(program, length, &length, log);

		fputs(log, stderr);
		exit(EXIT_FAILURE);
	}

	//use program object
	glUseProgram(program);
}

void draw(void)
{
	glUseProgram(0);
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		fputs("ERROR: No GLSL file specified.\n", stderr);
		exit(EXIT_FAILURE);
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutCreateWindow("glsl-preview");
	if (glewInit() != GLEW_OK)
	{
		fputs("ERROR: Failed to init GLEW.\n", stderr);
		exit(EXIT_FAILURE);
	}

	glClearColor(1.0, 0.0, 0.0, 0.0);
	gluOrtho2D(0.0, 0.0, (GLfloat)640, (GLfloat)480);


	glutDisplayFunc(draw);

	shader(argv[1]);
	glutMainLoop();

	return 0;
}
