#include <JEngine.h>

class ExampleLayer : public JEngine::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{
	}

	void OnUpdate() override
	{;
		JE_INFO("ExampleLayer::Update");
	}

	void OnEvent(JEngine::Event& event) override
	{
		JE_TRACE("{0}", event);
	}
};

class Sandbox : public JEngine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new JEngine::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

JEngine::Application* JEngine::CreateApplication()
{
	return new Sandbox();
}