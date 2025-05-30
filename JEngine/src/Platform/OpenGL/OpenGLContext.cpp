#include "jepch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace JEngine {

	OpenGLContext::OpenGLContext(GLFWwindow* window) : m_Window(window)
	{
		JE_CORE_ASSERT(window, "Window Handle is null!");
	}

	void OpenGLContext::Init()
	{
		JE_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		JE_CORE_ASSERT(status, "Failed to initialize Glad!");

		JE_CORE_INFO("OpenGL Info:");
		JE_CORE_INFO("	Vender: {0}", (const char*)glGetString(GL_VENDOR));
		JE_CORE_INFO("	Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		JE_CORE_INFO("	Version: {0}", (const char*)glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		JE_PROFILE_FUNCTION();

		glfwSwapBuffers(m_Window);
	}
};
