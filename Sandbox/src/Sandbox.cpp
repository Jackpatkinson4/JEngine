#include <JEngine.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public JEngine::Layer
{
public:
	ExampleLayer() 
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		// Vertex Array
		m_VertexArray.reset(JEngine::VertexArray::Create());

		// Vertex Buffer
		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
		};

		JEngine::Ref<JEngine::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(JEngine::VertexBuffer::Create(vertices, sizeof(vertices)));
		JEngine::BufferLayout layout = {
				{ JEngine::ShaderDataType::Float3, "a_Position"},
				{ JEngine::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		// Index Buffer
		unsigned int indices[3] = { 0, 1, 2 };

		JEngine::Ref<JEngine::IndexBuffer> indexBuffer;
		indexBuffer.reset(JEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(JEngine::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		JEngine::Ref<JEngine::VertexBuffer> squareVB;
		squareVB.reset(JEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ JEngine::ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		JEngine::Ref<JEngine::IndexBuffer> squareIB;
		squareIB.reset(JEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		// Shader (TBA)
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(JEngine::Shader::Create(vertexSrc, fragmentSrc));

		std::string vertexSrc2 = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc2 = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_Shader2.reset(JEngine::Shader::Create(vertexSrc2, fragmentSrc2));
	}

	void OnUpdate(JEngine::Timestep deltaTime) override
	{

		if (JEngine::Input::IsKeyPressed(JE_KEY_A) || JEngine::Input::IsKeyPressed(JE_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * deltaTime;
		else if (JEngine::Input::IsKeyPressed(JE_KEY_D) || JEngine::Input::IsKeyPressed(JE_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * deltaTime;

		if (JEngine::Input::IsKeyPressed(JE_KEY_S) || JEngine::Input::IsKeyPressed(JE_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * deltaTime;
		else if (JEngine::Input::IsKeyPressed(JE_KEY_W) || JEngine::Input::IsKeyPressed(JE_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * deltaTime;

		if (JEngine::Input::IsKeyPressed(JE_KEY_Z))
			m_CameraRotation += m_CameraRotationSpeed * deltaTime;
		else if (JEngine::Input::IsKeyPressed(JE_KEY_X))
			m_CameraRotation -= m_CameraRotationSpeed * deltaTime;

		JEngine::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
		JEngine::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		JEngine::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(0.1f));

		std::dynamic_pointer_cast<JEngine::OpenGLShader>(m_Shader2)->Bind();
		std::dynamic_pointer_cast<JEngine::OpenGLShader>(m_Shader2)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				JEngine::Renderer::Submit(m_Shader2, m_SquareVA, transform);
			}
		}

		JEngine::Renderer::Submit(m_Shader, m_VertexArray);

		JEngine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(JEngine::Event& event) override
	{
	}

private:
	JEngine::Ref<JEngine::Shader> m_Shader;
	JEngine::Ref<JEngine::VertexArray> m_VertexArray;

	JEngine::Ref<JEngine::Shader> m_Shader2;
	JEngine::Ref<JEngine::VertexArray> m_SquareVA;

	JEngine::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 2.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 90.0f;

	glm::vec3 m_SquareColor = { 0.2, 0.3, 0.8 };
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