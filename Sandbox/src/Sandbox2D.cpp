#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	JE_PROFILE_FUNCTION();

	m_Texture = JEngine::Texture2D::Create("assets/textures/misato_bread.png");
	m_TransparentTexture = JEngine::Texture2D::Create("assets/textures/sadness.png");
	m_TileMap = JEngine::Texture2D::Create("assets/textures/mainlev_build.png");

	m_RockTexture = JEngine::SubTexture2D::CreateFromCoords(m_TileMap, { 6.0f, 48.0f }, { 16.0f, 16.0f });
	m_ScaffoldTexture = JEngine::SubTexture2D::CreateFromCoords(m_TileMap, { 55.0f, 55.0f }, { 16.0f, 16.0f }, { 3.0f, 3.0f });
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
	JEngine::Renderer2D::ResetStats();
	{
		JE_PROFILE_SCOPE("Clear BG");
		JEngine::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
		JEngine::RenderCommand::Clear();
	}

	{
		JE_PROFILE_SCOPE("Renderer Draw");

		static float rotation = 0.0f;
		rotation += deltaTime * 50.0f;

		JEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
		//JEngine::Renderer2D::DrawQuad({ 1.5f, 0.0f }, { 1.0f, 1.0f }, m_SquareColor);
		//JEngine::Renderer2D::DrawRotatedQuad({ -1.5f, -0.5f }, { 0.5f, 0.75f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
		//JEngine::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1 }, { 20.0f, 20.0f }, m_TransparentTexture, 10.f);
		//JEngine::Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_Texture, 1.0f, m_TextureBlendColor);
		//JEngine::Renderer2D::EndScene();

		//JEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
		//for (float y = -5.0f; y < 5.0f; y += 0.5f)
		//{
		//	for (float x = -5.0f; x < 5.0f; x += 0.5f)
		//	{
		//		glm::vec4 color = { (x + 5.0f) / 10.0f, 0.5f, (x + 5.0f) / 10.0f, 0.5f };
		//		JEngine::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
		//	}
		//}

		for (int i = -10; i < 10; i++)
		{
			JEngine::Renderer2D::DrawQuad({ i, 0.0f }, { 1.0f, 1.0f }, m_RockTexture);
		}
		JEngine::Renderer2D::DrawQuad({ 1.0f, 2.0f }, { 3.0f, 3.0f }, m_ScaffoldTexture);

		JEngine::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	JE_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = JEngine::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.drawCalls);
	ImGui::Text("Quads: %d", stats.quadCount);
	ImGui::Text("Triangles: %d", stats.GetTotalTriangleCount());
	ImGui::Text("Verticies: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indicies: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::ColorEdit3("Texture Blend Color", glm::value_ptr(m_TextureBlendColor));

	ImGui::End();
}

void Sandbox2D::OnEvent(JEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}