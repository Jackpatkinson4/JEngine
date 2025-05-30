#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	m_VertexArray = JEngine::VertexArray::Create();

	float vertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
	};

	JEngine::Ref<JEngine::VertexBuffer> vertexBuffer;
	vertexBuffer.reset(JEngine::VertexBuffer::Create(vertices, sizeof(vertices)));
	vertexBuffer->SetLayout({
		{ JEngine::ShaderDataType::Float3, "a_Position"},
	});
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

	JEngine::Ref<JEngine::IndexBuffer> indexBuffer;
	indexBuffer.reset(JEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	m_Shader = JEngine::Shader::Create("assets/shaders/FlatColor.glsl");
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(JEngine::Timestep deltaTime)
{
	// Update
	m_CameraController.OnUpdate(deltaTime);

	// Render
	JEngine::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
	JEngine::RenderCommand::Clear();

	JEngine::Renderer::BeginScene(m_CameraController.GetCamera());

	std::dynamic_pointer_cast<JEngine::OpenGLShader>(m_Shader)->Bind();
	std::dynamic_pointer_cast<JEngine::OpenGLShader>(m_Shader)->UploadUniformFloat4("u_Color", m_SquareColor);

	JEngine::Renderer::Submit(m_Shader, m_VertexArray, glm::scale(glm::mat4(1.0), glm::vec3(1.5f)));

	JEngine::Renderer::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(JEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}