#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	JE_PROFILE_FUNCTION();

	m_Texture = JEngine::Texture2D::Create("assets/textures/misato_bread.png");
}

void Sandbox2D::OnDetach()
{
	JE_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(JEngine::Timestep deltaTime)
{
	JE_PROFILE_FUNCTION();

	// Update
	{
		m_CameraController.OnUpdate(deltaTime);
	}

	// Render
	{
		JE_PROFILE_SCOPE("Clear BG");
		JEngine::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
		JEngine::RenderCommand::Clear();
	}

	{
		JE_PROFILE_SCOPE("Renderer Draw");
		JEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());

		JEngine::Renderer2D::DrawRotatedQuad({ 1.0f, 1.0f }, { 1.0f, 1.0f }, glm::radians(45.0f), m_SquareColor);
		JEngine::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_Texture, 10.0f, m_TextureBlendColor);

		JEngine::Renderer::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	JE_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::ColorEdit3("Texture Blend Color", glm::value_ptr(m_TextureBlendColor));

	ImGui::End();
}

void Sandbox2D::OnEvent(JEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}