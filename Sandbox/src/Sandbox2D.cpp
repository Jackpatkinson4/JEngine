#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const uint32_t s_MapWidth = 24;
static const char* s_MapTiles =
"AAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAGGGGGGGGGGGG"
"AAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAA"
"GGGGGGGGGGGGGGGGGGGGGGGG"
"AAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAA"
;

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	JE_PROFILE_FUNCTION();

	m_Texture = JEngine::Texture2D::Create("assets/textures/misato_bread.png");
	m_TransparentTexture = JEngine::Texture2D::Create("assets/textures/sadness.png");
	m_TileMap = JEngine::Texture2D::Create("assets/textures/mainlev_build.png");

	m_TextureMap['G'] = JEngine::SubTexture2D::CreateFromCoords(m_TileMap, { 6.0f, 48.0f }, { 16.0f, 16.0f });
	m_TextureMap['A'] = JEngine::SubTexture2D::CreateFromCoords(m_TileMap, { 6.0f, 16.0f }, { 16.0f, 16.0f });

	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / s_MapWidth;
	m_ScaffoldTexture = JEngine::SubTexture2D::CreateFromCoords(m_TileMap, { 55.0f, 55.0f }, { 16.0f, 16.0f }, { 3.0f, 3.0f });

	m_CameraController.SetZoomLevel(5.0f);

	JEngine::FramebufferSpecification fbSpec;
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
	m_Framebuffer = JEngine::Framebuffer::Create(fbSpec);
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
		m_Framebuffer->Bind();
		JEngine::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
		JEngine::RenderCommand::Clear();
	}

	{
		JE_PROFILE_SCOPE("Renderer Draw");

		static float rotation = 0.0f;
		rotation += deltaTime * 50.0f;

		JEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
		JEngine::Renderer2D::DrawQuad({ 1.5f, 0.0f }, { 1.0f, 1.0f }, m_SquareColor);
		JEngine::Renderer2D::DrawRotatedQuad({ -1.5f, -0.5f }, { 0.5f, 0.75f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
		JEngine::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1 }, { 20.0f, 20.0f }, m_TransparentTexture, 10.f);
		JEngine::Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_Texture, 1.0f, m_TextureBlendColor);
		JEngine::Renderer2D::EndScene();

		JEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.5f, (x + 5.0f) / 10.0f, 0.5f };
				JEngine::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}

		/*for (uint32_t y = 0; y < m_MapHeight; y++)
		{
			for (uint32_t x = 0; x < m_MapWidth; x++)
			{
				//JEngine::Renderer2D::DrawQuad({ i, 0.0f }, { 1.0f, 1.0f }, m_RockTexture);

				char tileType = s_MapTiles[x + y * m_MapWidth];
				JEngine::Ref<JEngine::SubTexture2D> texture;

				if (m_TextureMap.find(tileType) != m_TextureMap.end())
					texture = m_TextureMap[tileType];
				else
					texture = m_ScaffoldTexture;


				JEngine::Renderer2D::DrawQuad({ x - m_MapWidth / 2.0f, m_MapHeight - y - m_MapHeight / 2.0f, 0.0f }, { 1.0f, 1.0f }, texture);
			}
		}
		JEngine::Renderer2D::DrawQuad({ 1.0f, 2.0f }, { 3.0f, 3.0f }, m_ScaffoldTexture); */

		JEngine::Renderer2D::EndScene();
		m_Framebuffer->Unbind();
	}
}

void Sandbox2D::OnImGuiRender()
{
	JE_PROFILE_FUNCTION();

	static bool dockingEnabled = true;

	if (dockingEnabled) 
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				if (ImGui::MenuItem("Exit")) JEngine::Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

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

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ 1280.0f, 720.0f }, ImVec2{0,1}, ImVec2{1,0});
		ImGui::End();

		ImGui::End();
	}
	else
	{
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

		uint32_t textureID = m_Texture->GetRendererID();
		ImGui::Image((void*)textureID, ImVec2(256.0f, 256.0f), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
	}
}

void Sandbox2D::OnEvent(JEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}