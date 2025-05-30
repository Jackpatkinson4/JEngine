#pragma once

#include "JEngine/Core/Layer.h"

#include "JEngine/Events/ApplicationEvent.h"
#include "JEngine/Events/KeyEvent.h"
#include "JEngine/Events/MouseEvent.h"

namespace JEngine {

	class JE_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		ImGuiLayer(bool show);
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
		bool m_Show = false;
	};
}
