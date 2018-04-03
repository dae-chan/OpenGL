#include <GL/glew.h>

void OnAwake()
{
	glClearColor(0, 0, 1, 1);
}

void OnUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT);
}