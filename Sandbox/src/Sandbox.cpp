#include <JEngine.h>

#include "imgui/imgui.h"

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

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
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
	}

	~Sandbox()
	{

	}
};

JEngine::Application* JEngine::CreateApplication()
{
	return new Sandbox();
}