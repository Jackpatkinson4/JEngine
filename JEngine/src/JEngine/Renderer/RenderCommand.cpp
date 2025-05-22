#include "jepch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace JEngine{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}