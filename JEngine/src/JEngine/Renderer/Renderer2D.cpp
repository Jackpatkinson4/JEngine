#include "jepch.h"
#include "JEngine/Renderer/Renderer2D.h"

#include "JEngine/Renderer/VertexArray.h"
#include "JEngine/Renderer/Shader.h"
#include "JEngine/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace JEngine {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
	};

	struct Renderer2DData
	{
		static const uint32_t maxQuads = 10000;
		static const uint32_t maxVertices = maxQuads * 4;
		static const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxTextureSlots = 32; // TODO: RendrCaps

		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> textureShader;
		Ref<Texture2D> baseTexture;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1; // 0 - white texture

		glm::vec4 quadVertexPositions[4];

		Renderer2D::Statistics stats;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		JE_PROFILE_FUNCTION();

		s_Data.quadVertexArray = VertexArray::Create();

		s_Data.quadVertexBuffer = VertexBuffer::Create(s_Data.maxVertices * sizeof(QuadVertex));
		s_Data.quadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" }
		});
		s_Data.quadVertexArray->AddVertexBuffer(s_Data.quadVertexBuffer);

		s_Data.quadVertexBufferBase = new QuadVertex[s_Data.maxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.maxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.maxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4; 
		}

		Ref<IndexBuffer> quadIndexBuffer = IndexBuffer::Create(quadIndices, s_Data.maxIndices);
		s_Data.quadVertexArray->SetIndexBuffer(quadIndexBuffer);
		delete[] quadIndices;

		s_Data.baseTexture = Texture2D::Create(1, 1);
		uint32_t baseTextureData = 0xffffffff;
		s_Data.baseTexture->SetData(&baseTextureData, sizeof(uint32_t));

		int samplers[s_Data.maxTextureSlots];
		for (uint32_t i = 0; i < s_Data.maxTextureSlots; i++)
			samplers[i] = i;

		s_Data.textureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.textureShader->Bind();
		s_Data.textureShader->SetIntArray("u_Textures", samplers, s_Data.maxTextureSlots);

		s_Data.textureSlots[0] = s_Data.baseTexture;

		s_Data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		JE_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		JE_PROFILE_FUNCTION();

		s_Data.textureShader->Bind();
		s_Data.textureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data.quadIndexCount = 0;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

		s_Data.textureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		JE_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_Data.quadVertexBufferPtr - (uint8_t*)s_Data.quadVertexBufferBase;
		s_Data.quadVertexBuffer->SetData(s_Data.quadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		// Bind textures
		for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
		{
			s_Data.textureSlots[i]->Bind(i);
		}

		RenderCommand::DrawIndexed(s_Data.quadVertexArray, s_Data.quadIndexCount);

		s_Data.stats.drawCalls++;
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_Data.quadIndexCount = 0;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

		s_Data.textureSlotIndex = 1;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		JE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Data.quadIndexCount >= s_Data.maxIndices)
			FlushAndReset();

		const float textureIndex = 0.0f;
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.quadVertexBufferPtr->Position = transform * s_Data.quadVertexPositions[i];
			s_Data.quadVertexBufferPtr->Color = color;
			s_Data.quadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.quadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.quadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.quadVertexBufferPtr++;
		}

		s_Data.quadIndexCount += 6;

		s_Data.stats.quadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		JE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

		if (s_Data.quadIndexCount >= s_Data.maxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
		{
			if (*s_Data.textureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.textureSlotIndex;
			s_Data.textureSlots[s_Data.textureSlotIndex] = texture;
			s_Data.textureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.quadVertexBufferPtr->Position = transform * s_Data.quadVertexPositions[i];
			s_Data.quadVertexBufferPtr->Color = tintColor;
			s_Data.quadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.quadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.quadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.quadVertexBufferPtr++;
		}

		s_Data.quadIndexCount += 6;

		s_Data.stats.quadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		JE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		if (s_Data.quadIndexCount >= s_Data.maxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
		{
			if (*s_Data.textureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.textureSlotIndex;
			s_Data.textureSlots[s_Data.textureSlotIndex] = texture;
			s_Data.textureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.quadVertexBufferPtr->Position = transform * s_Data.quadVertexPositions[i];
			s_Data.quadVertexBufferPtr->Color = tintColor;
			s_Data.quadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.quadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.quadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.quadVertexBufferPtr++;
		}

		s_Data.quadIndexCount += 6;

		s_Data.stats.quadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		JE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Data.quadIndexCount >= s_Data.maxIndices)
			FlushAndReset();

		const float textureIndex = 0.0f;
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.quadVertexBufferPtr->Position = transform * s_Data.quadVertexPositions[i];
			s_Data.quadVertexBufferPtr->Color = color;
			s_Data.quadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.quadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.quadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.quadVertexBufferPtr++;
		}

		s_Data.quadIndexCount += 6;

		s_Data.stats.quadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		JE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Data.quadIndexCount >= s_Data.maxIndices)
			FlushAndReset();


		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
		{
			if (*s_Data.textureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.textureSlotIndex;
			s_Data.textureSlots[s_Data.textureSlotIndex] = texture;
			s_Data.textureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.quadVertexBufferPtr->Position = transform * s_Data.quadVertexPositions[i];
			s_Data.quadVertexBufferPtr->Color = tintColor;
			s_Data.quadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.quadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.quadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.quadVertexBufferPtr++;
		}

		s_Data.quadIndexCount += 6;

		s_Data.stats.quadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		JE_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		if (s_Data.quadIndexCount >= s_Data.maxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
		{
			if (*s_Data.textureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.textureSlotIndex;
			s_Data.textureSlots[s_Data.textureSlotIndex] = texture;
			s_Data.textureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.quadVertexBufferPtr->Position = transform * s_Data.quadVertexPositions[i];
			s_Data.quadVertexBufferPtr->Color = tintColor;
			s_Data.quadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.quadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.quadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.quadVertexBufferPtr++;
		}

		s_Data.quadIndexCount += 6;

		s_Data.stats.quadCount++;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.stats;
	}
}