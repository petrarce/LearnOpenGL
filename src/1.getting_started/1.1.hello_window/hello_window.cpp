#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLGlfwContextCreator.hpp>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
using namespace glcontext::creators;

int main()
{
	GLGlfwContextCreator glfwContext;
	GLContextBinder bind(glfwContext);
	// glfw: initialize and configure
	// ------------------------------
	while (!glfwContext.processWindow<int>(glfwWindowShouldClose))
	{
		// input
		// -----
		glfwContext.processWindow<void>(processInput);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwContext.processWindow<void>(glfwSwapBuffers);
		glfwPollEvents();
	}

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
