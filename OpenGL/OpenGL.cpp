#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

extern void onAwake();
extern void onUpdate(double current_time);
extern void onShutdown();
extern int getWindowWidth();
extern int getWindowHeight();

void window_resized(GLFWwindow* window, int width, int height);
void key_pressed(GLFWwindow* window, int key, int scancode, int action, int mods);
void show_glfw_error(int error, const char* description);

int main()
{
	glfwSetErrorCallback(show_glfw_error);

	if (!glfwInit()) {
		std::cerr << "GLFW initialize failure" << '\n';
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(
		getWindowWidth(), // width
		getWindowHeight(), // height
		"OpenGL Example",
		NULL, NULL);

	if (!window)
	{
		std::cerr << "Window create failure" << '\n';
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, window_resized);
	glfwSetKeyCallback(window, key_pressed);
	glfwSwapInterval(1);
	glewExperimental = GL_TRUE;

	GLenum err = glewInit();

	if (err != GLEW_OK) {
		std::cerr << "GLEW initialize failure " << glewGetErrorString(err) << '\n';
		glfwTerminate();
		exit(-1);
	}

	onAwake();

	while (!glfwWindowShouldClose(window)) {
		onUpdate(glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	onShutdown();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
void show_glfw_error(int error, const char* description) {
	std::cerr << "Error: " << description << '\n';
}
void window_resized(GLFWwindow* window, int width, int height) {
	std::cout << "Window resized, new window size: " << width << " x " << height << '\n';
	glClearColor(0, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
}
void key_pressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == 'Q' && action == GLFW_PRESS) {
		glfwTerminate();
		exit(0);
	}
}