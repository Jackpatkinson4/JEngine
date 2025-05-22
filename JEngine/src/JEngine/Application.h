#pragma once

#include "Core.h"

#include "Window.h"
#include "JEngine/LayerStack.h"
#include "JEngine/Events/Event.h"
#include "JEngine/Events/ApplicationEvent.h"

#include "JEngine/ImGui/ImGuiLayer.h"

#include "JEngine/Renderer/Shader.h"
#include "JEngine/Renderer/Buffer.h"
#include "JEngine/Renderer/VertexArray.h"

namespace JEngine {

	class JE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;

		std::shared_ptr<Shader> m_Shader2;
		std::shared_ptr<VertexArray> m_SquareVA;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

