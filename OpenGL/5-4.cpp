#include "OpenGL.h"

#ifdef EX5_4

#include <vmath.h>

GLuint program;
GLuint vao;
GLuint position_buffer;
GLuint index_buffer;
GLint mv_location;
GLint proj_location;

int getWindowWidth()
{
	return 800;
}

int getWindowHeight()
{
	return 600;
}

void onAwake()
{
	static const char * vs_source[] =
	{
		"#version 420 core                                                  \n"
		"                                                                   \n"
		"in vec4 position;                                                  \n"
		"                                                                   \n"
		"out VS_OUT                                                         \n"
		"{                                                                  \n"
		"    vec4 color;                                                    \n"
		"} vs_out;                                                          \n"
		"                                                                   \n"
		"uniform mat4 mv_matrix;                                            \n"
		"uniform mat4 proj_matrix;                                          \n"
		"                                                                   \n"
		"void main(void)                                                    \n"
		"{                                                                  \n"
		"    gl_Position = proj_matrix * mv_matrix * position;              \n"
		"    vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);      \n"
		"}                                                                  \n"
	};

	static const char * fs_source[] =
	{
		"#version 420 core                                                  \n"
		"                                                                   \n"
		"out vec4 color;                                                    \n"
		"                                                                   \n"
		"in VS_OUT                                                          \n"
		"{                                                                  \n"
		"    vec4 color;                                                    \n"
		"} fs_in;                                                           \n"
		"                                                                   \n"
		"void main(void)                                                    \n"
		"{                                                                  \n"
		"    color = fs_in.color;                                           \n"
		"}                                                                  \n"
	};

	program = glCreateProgram();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_source, NULL);
	glCompileShader(fs);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_source, NULL);
	glCompileShader(vs);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	mv_location = glGetUniformLocation(program, "mv_matrix");
	proj_location = glGetUniformLocation(program, "proj_matrix");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	static const GLushort vertex_indices[] =
	{
		0, 1, 2,
		2, 1, 3,
		2, 3, 4,
		4, 3, 5,
		4, 5, 6,
		6, 5, 7,
		6, 7, 0,
		0, 7, 1,
		6, 0, 2,
		2, 4, 6,
		7, 5, 3,
		7, 3, 1
	};

	static const GLfloat vertex_positions[] =
	{
		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,
		0.25f, -0.25f, -0.25f,
		0.25f,  0.25f, -0.25f,
		0.25f, -0.25f,  0.25f,
		0.25f,  0.25f,  0.25f,
		-0.25f, -0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
	};

	glGenBuffers(1, &position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertex_positions),
		vertex_positions,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(vertex_indices),
		vertex_indices,
		GL_STATIC_DRAW);

	glEnable(GL_CULL_FACE);
	// glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void onUpdate(double current_time)
{
	int i;
	static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
	static const GLfloat one = 1.0f;

	glViewport(0, 0, getWindowWidth(), getWindowHeight());
	glClearBufferfv(GL_COLOR, 0, green);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glUseProgram(program);

	vmath::mat4 proj_matrix = vmath::perspective(50.0f,
		(float)getWindowWidth() / (float)getWindowHeight(),
		0.1f,
		1000.0f);
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);

#ifdef MANY_CUBES
	for (i = 0; i < 24; i++)
	{
		float f = (float)i + (float)current_time * 0.3f;
		vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -20.0f) *
			vmath::rotate((float)current_time * 45.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float)current_time * 21.0f, 1.0f, 0.0f, 0.0f) *
			vmath::translate(sinf(2.1f * f) * 2.0f,
				cosf(1.7f * f) * 2.0f,
				sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
	}
#else
	float f = (float)current_time * 0.3f;
	vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -4.0f) *
		vmath::translate(sinf(2.1f * f) * 0.5f,
			cosf(1.7f * f) * 0.5f,
			sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
		vmath::rotate((float)current_time * 45.0f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate((float)current_time * 81.0f, 1.0f, 0.0f, 0.0f);
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
#endif
}

void onShutdown()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(program);
	glDeleteBuffers(1, &position_buffer);
}
#endif
