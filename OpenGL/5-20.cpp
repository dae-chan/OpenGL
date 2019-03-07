#include "OpenGL.h"

#ifdef EX5_20

GLuint program;
GLuint buffer;

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
		"#version 430 core                                                  \n"
		"                                                                   \n"
		"layout (binding = 0) uniform area_block                            \n"
		"{                                                                  \n"
		"    uint counter_value;                                            \n"
		"};                                                                 \n"
		"                                                                   \n"
		"out vec4 color;                                                    \n"
		"                                                                   \n"
		"uniform float max_area;                                            \n"
		"                                                                   \n"
		"void main(void)                                                    \n"
		"{                                                                  \n"
		"    float brightness = clamp(float(counter_value) / max_area,      \n"
		"                       0.0, 1.0);                                  \n"
		"    color = vec4(brightness, brightness, brightness, 1.0);         \n"
		"}                                                                  \n"
	};

	static const char * fs_source[] =
	{
		"#version 430 core                                                  \n"
		"                                                                   \n"
		"layout (binding = 0, offset = 0) uniform atomic_uint area;         \n"
		"                                                                   \n"
		"void main(void)                                                    \n"
		"{                                                                  \n"
		"    atomicCounterIncrement(area);                                  \n"
		"}                                                                  \n"
	};

	program = glCreateProgram();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_source, NULL);
	glCompileShader(fs);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_source, NULL);
	glCompileShader(vs);

	CheckShaderCompileError(fs);
	CheckShaderCompileError(vs);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	glGenBuffers(1, &buffer);

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, 16 * sizeof(GLuint), NULL, GL_DYNAMIC_COPY);

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 3, buffer);

	const GLuint zero = 0;
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);

	// Method 1 - Reset atomic counter using glBufferSubData.
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 2 * sizeof(GLuint), sizeof(GLuint), &zero);

	// Method 2 - Buffer mapping
	//GLuint *data = (GLuint *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, 16 * sizeof(GLuint), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
	//data[2] = 0;
	//glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

	// Method 3 - glCelarBufferSubData
	//glClearBufferSubData(GL_ATOMIC_COUNTER_BUFFER,
	//	GL_R32UI,
	//	2 * sizeof(GLuint),
	//	sizeof(GLuint),
	//	GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);
}



void onUpdate(double current_time)
{
}

void onShutdown()
{
	glDeleteProgram(program);
	glDeleteBuffers(1, &buffer);
}

#endif
