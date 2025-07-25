#include "ExampleLayer.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Platform/OpenGL/OpenGLShader.h>


ExampleLayer::ExampleLayer() : Layer("Example"), m_CameraController(1280.0f / 720.0f)
{
	// Vertex Array
	m_VertexArray = JEngine::VertexArray::Create();

	// Vertex Buffer
	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
	};

	JEngine::Ref<JEngine::VertexBuffer> vertexBuffer;
	vertexBuffer = JEngine::VertexBuffer::Create(vertices, sizeof(vertices));
	JEngine::BufferLayout layout = {
			{ JEngine::ShaderDataType::Float3, "a_Position"},
			{ JEngine::ShaderDataType::Float4, "a_Color" }
	};
	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	// Index Buffer
	unsigned int indices[3] = { 0, 1, 2 };

	JEngine::Ref<JEngine::IndexBuffer> indexBuffer;
	indexBuffer = JEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	m_SquareVA = JEngine::VertexArray::Create();

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	JEngine::Ref<JEngine::VertexBuffer> squareVB;
	squareVB = JEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout({
		{ JEngine::ShaderDataType::Float3, "a_Position" },
		{ JEngine::ShaderDataType::Float2, "a_TexCoord" }
		});
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	JEngine::Ref<JEngine::IndexBuffer> squareIB;
	squareIB = JEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
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

	m_Shader = JEngine::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

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

	m_Shader2 = JEngine::Shader::Create("FlatColor", vertexSrc2, fragmentSrc2);

	auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

	m_Texture = JEngine::Texture2D::Create("assets/textures/misato_bread.png");
	m_Transparent_Texture = JEngine::Texture2D::Create("assets/textures/sadness.png");

	std::dynamic_pointer_cast<JEngine::OpenGLShader>(textureShader)->Bind();
	std::dynamic_pointer_cast<JEngine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
}

void ExampleLayer::OnUpdate(JEngine::Timestep deltaTime)
{
	// Update
	m_CameraController.OnUpdate(deltaTime);

	// Render
	JEngine::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.2f });
	JEngine::RenderCommand::Clear();

	JEngine::Renderer::BeginScene(m_CameraController.GetCamera());

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

	auto textureShader = m_ShaderLibrary.Get("Texture");

	m_Texture->Bind();
	JEngine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0), glm::vec3(1.5f)));
	m_Transparent_Texture->Bind();
	JEngine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0), glm::vec3(1.5f)));

	//Triangle
	//JEngine::Renderer::Submit(m_Shader, m_VertexArray);

	JEngine::Renderer::EndScene();
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void ExampleLayer::OnEvent(JEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}