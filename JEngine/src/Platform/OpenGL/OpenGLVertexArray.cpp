#include "jepch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace JEngine {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case JEngine::ShaderDataType::Float:    return GL_FLOAT;
			case JEngine::ShaderDataType::Float2:   return GL_FLOAT;
			case JEngine::ShaderDataType::Float3:   return GL_FLOAT;
			case JEngine::ShaderDataType::Float4:   return GL_FLOAT;
			case JEngine::ShaderDataType::Mat3:     return GL_FLOAT;
			case JEngine::ShaderDataType::Mat4:     return GL_FLOAT;
			case JEngine::ShaderDataType::Int:      return GL_INT;
			case JEngine::ShaderDataType::Int2:     return GL_INT;
			case JEngine::ShaderDataType::Int3:     return GL_INT;
			case JEngine::ShaderDataType::Int4:     return GL_INT;
			case JEngine::ShaderDataType::Bool:     return GL_BOOL;
		}
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		JE_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		JE_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		JE_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		JE_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		JE_PROFILE_FUNCTION();

		JE_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		JE_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}