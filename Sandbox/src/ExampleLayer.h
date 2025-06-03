#pragma once

#include <JEngine.h>

class ExampleLayer : public JEngine::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	void OnUpdate(JEngine::Timestep deltaTime) override;

	virtual void OnImGuiRender() override;

	void OnEvent(JEngine::Event& e) override;

private:
	JEngine::ShaderLibrary m_ShaderLibrary;
	JEngine::Ref<JEngine::Shader> m_Shader;
	JEngine::Ref<JEngine::VertexArray> m_VertexArray;

	JEngine::Ref<JEngine::Shader> m_Shader2;
	JEngine::Ref<JEngine::VertexArray> m_SquareVA;

	JEngine::Ref<JEngine::Texture2D> m_Texture, m_Transparent_Texture;

	JEngine::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquareColor = { 0.2, 0.3, 0.8 };
};
