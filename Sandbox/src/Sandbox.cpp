#include <JEngine.h>

class ExampleLayer : public JEngine::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{
	}

	void OnUpdate() override
	{;
		if (JEngine::Input::IsKeyPressed(JE_KEY_TAB))
			JE_TRACE("Tab key is pressed! (poll)");
	}

	void OnEvent(JEngine::Event& event) override
	{
		if (event.GetEventType() == JEngine::EventType::KeyPressed)
		{
			JEngine::KeyPressedEvent& e = (JEngine::KeyPressedEvent&)event;
			if (e.GetKeyCode() == JE_KEY_TAB)
			{
				JE_TRACE("Tab key is pressed! (event)");
			}
			JE_TRACE("{0}", (char)e.GetKeyCode());
		}
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