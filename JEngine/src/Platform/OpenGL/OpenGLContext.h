#pragma once

#include "JEngine/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace JEngine {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* window);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_Window;
	};
}

