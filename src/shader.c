#include "shader.h"

#include <string.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>

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

int check_compile_status(int shader_id, const char* path, char* error_message)
{
	GLint status = GL_FALSE;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length = 0;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
		char log[length];
		glGetShaderInfoLog(shader_id, length, &length, log);

		sprintf(error_message, "Shader Error in %s:\n%s", path, log);
		return 0;
	}

	return 1;
}

int check_link_status(int program_id, const char* path, char* error_message)
{
	GLuint status = GL_FALSE;
	glGetProgramiv(program_id, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length = 0;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
		char log[length];
		glGetProgramInfoLog(program_id, length, &length, log);

		sprintf(error_message, "Shader Error in %s:\n%s", path, log);
		return 0;
	}

	return 1;
}

GLuint vertex_shader(const char* path, int* status, char* error_message, int raw_source)
{
	*status = GL_TRUE;
	const char* shader_source;
	if (!raw_source)
	{
		shader_source = load_shader(path);
		if (!shader_source)
		{
			sprintf(error_message, "Failed to load \"%s\"\n", path);
			*status = GL_FALSE;
			return 0;
		}
	}
	else
	{
		shader_source = path;
	}

	GLuint shader_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader_id, 1, &shader_source, NULL);
	glCompileShader(shader_id);
	*status = check_compile_status(shader_id, path, error_message);

	return shader_id;
}

GLuint fragment_shader(const char* path, int* status, char* error_message, int raw_source)
{
	*status = GL_TRUE;
	const char* shader_source;
	if (!raw_source)
	{
		shader_source = load_shader(path);
		if (!shader_source)
		{
			sprintf(error_message, "Failed to load \"%s\"\n", path);
			*status = GL_FALSE;
			return 0;
		}
	}
	else
	{
		shader_source = path;
	}

	GLuint shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	//Compile the shader
	glShaderSource(shader_id, 1, &shader_source, NULL);
	glCompileShader(shader_id);
	*status = check_compile_status(shader_id, path, error_message);

	return shader_id;
}

int shader(const char* path, int* status, char* error_message, int raw_source)
{
	strcpy(error_message, "\0");

	const char vshader[] =
	"#version 130\n"
	"in mediump vec3 point;\n"
	"in mediump vec2 texcoord;\n"
	"out mediump vec2 UV;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(point, 1);\n"
	"	UV = texcoord;\n"
	"}\n";

	//Create vertex & fragment shaders
	GLuint vertex = vertex_shader(vshader, status, error_message, 1);
	if (*status == GL_FALSE) return 0;

	GLuint fragment = fragment_shader(path, status, error_message, raw_source);
	if (*status == GL_FALSE) return 0;

	//Attach them to program.
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);

	//link and error check
	glLinkProgram(program);
	*status = check_link_status(program, path, error_message);
	if (*status == GL_FALSE) return 0;

	GLfloat vertices[] = {
		1.0f,  1.0f,  0.0f,
		-1.0f,  1.0f,  0.0f,
		-1.0f, -1.0f,  0.0f,
		1.0f, -1.0f,  0.0f
	};
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	float pixels[] = {
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f
	};

	//Send vertices to GPU
	GLuint vert_array;
	glGenVertexArrays(1, &vert_array);
	glBindVertexArray(vert_array);

	//vertex buffers???
	GLuint vert_buf;
	glGenBuffers(1, &vert_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint index_buf;
	glGenBuffers(1, &index_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(glGetAttribLocation(program, "point"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glVertexAttribPointer(glGetAttribLocation(program, "texcoord"), 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(3 * sizeof(float)));

	glEnable(GL_DEPTH_TEST);

	//use program object
	// glUseProgram(program);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_BGR, GL_FLOAT, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	return program;
}
