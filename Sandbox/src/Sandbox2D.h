#pragma once

#include "JEngine.h"

class Sandbox2D : public JEngine::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(JEngine::Timestep deltaTime) override;
	virtual void OnImGuiRender() override;
	void OnEvent(JEngine::Event& e) override;

private:
	JEngine::OrthographicCameraController m_CameraController;

	// Temp
	JEngine::Ref<JEngine::VertexArray> m_VertexArray;
	JEngine::Ref<JEngine::Shader> m_Shader;

	JEngine::Ref<JEngine::Texture2D> m_Texture;
	JEngine::Ref<JEngine::Texture2D> m_TransparentTexture;
	JEngine::Ref<JEngine::Texture2D> m_TileMap;
	JEngine::Ref<JEngine::SubTexture2D> m_RockTexture;
	JEngine::Ref<JEngine::SubTexture2D> m_ScaffoldTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	glm::vec4 m_TextureBlendColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};
