#include "pch.h"
#include "Render2D.h"

#include "GL/GL_Include.h"
#include "GL/gl_error_macro_db.h"
#include "glad/glad.h"

//qptr->indexBuffer = IndexBuffer::CreateIndexBuffer(_msize(indices), indices);

#ifdef GAME_WINDOWS_BUILD

#include <malloc.h>
#define GAME_MSIZE(x) malloc_usable_size(x)

#else

#include <malloc.h>
#define GAME_MSIZE(x) malloc_usable_size(x)

#endif

namespace Game
{
	namespace utils
	{
		/* Do a check on the pointer using a if statement, if true, delete and set to nullptr */
		static inline void Delete(void* ptr)
		{
			if (ptr)
			{
				delete ptr;
				ptr = nullptr;
			}
		}
		/* Do a check on the pointer using a if statement, if true, free and set to nullptr */
		static inline void Free(void* ptr)
		{
			if (ptr)
			{
				free(ptr);
				ptr = nullptr;
			}
		}

		inline static void SampleTextureOnShader(Ref<Shader>& shader, int32_t max_textures, std::vector<uint32_t>& slots)
		{
			int32_t* samplers = new int32_t[max_textures];
			for (int i = 0; i < max_textures; i++)
				samplers[i] = i;
			shader->SetUniform1iv("u_Textures", max_textures, samplers);
			delete[] samplers;

			slots = std::vector<uint32_t>(max_textures);
			slots[0] = Texture::GetWhiteTexture()->GetId();
			for (size_t i = 1; i < max_textures; i++)
				slots[i] = 0;
		}
	}

	static bool s_RenderInit = false;
	static int32_t QuadMaxCount = 500;
	static int32_t QuadMaxVertexCount = QuadMaxCount * 4;
	static int32_t QuadMaxIndexCount = QuadMaxCount * 6;
	static Scope<ShaderLib> m_ShaderLib;

	static glm::vec4 QuadVertexPositions[4] =
	{
		{ -0.5f, -0.5f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, 0.0f, 1.0f },
		{ 0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 1.0f }
	};

	static glm::vec2 DefaultTextureCoords[] = {
		{0.0f,0.0f},
		{1.0f,0.0f},
		{1.0f,1.0f},
		{0.0f,1.0f}
	};

	struct RenderStats
	{
		DrawInfo GeometryType;
		uint64_t GeometryCounterPerFrame;
		uint64_t GeometryCounterLifeSpam;
	};

	struct QuadVertexData
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoords;
		float TextureIndex;
	};

	struct DrawGeometryData
	{
		//Vertex stuff
		Ref<VertexArray> vertexArray;
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;

		//Texture stuff
		std::vector<uint32_t> TexturesID;
		uint32_t TextureSlotIndex = 1;
		static const uint8_t WhiteTextureID = 1; //

		//Shader stuff
		Ref<Shader> shader;

		//Buffer stuff
		void* Buffer = nullptr;
		void* BufferPtr = nullptr;

		//Counter
		uint32_t IndexCount = 0;

		//Metrics
		RenderStats Stats;

		//Utils
		GLsizeiptr SizePtr() const { return (uint8_t*)BufferPtr - (uint8_t*)Buffer; }
		void Dispose() 
		{ 
			//Temp
			vertexArray->Dispose();
			vertexBuffer->Dispose();
			indexBuffer->Dispose();
			TexturesID.clear();
			shader.~shared_ptr();
		}
	};

	// All info about Quads are here

	struct DrawDataCollection
	{
		DrawGeometryData* Quads = nullptr;
	};

	static DrawDataCollection s_Data;

	bool Render2D::Init()
	{
		if (s_RenderInit)
			return s_RenderInit;

		s_RenderInit = true;
		//Shader is managed as a Ref(shared_ptr) inside ShaderLib
		m_ShaderLib = MakeScope<ShaderLib>();
		auto quadShader = Shader::CreateShader("assets/shaders/Quad.glsl");
		m_ShaderLib->Add(quadShader);

		//Create Quad GL stuff
		s_Data.Quads = new DrawGeometryData();
		auto qptr = s_Data.Quads; /* Handler pointer to a easier name */

		//Init Quad stuff
		qptr->shader = quadShader;
		qptr->shader->Bind();
		
		qptr->Buffer = new QuadVertexData[QuadMaxVertexCount];
		qptr->BufferPtr = qptr->Buffer;

		qptr->vertexArray = VertexArray::CreateVertexArray();

		qptr->vertexBuffer = VertexBuffer::CreateVertexBuffer(QuadMaxVertexCount * sizeof(QuadVertexData));

		VertexAttribute layout(qptr->vertexBuffer);

		layout.AddLayoutFloat(3, sizeof(QuadVertexData), (const void*)offsetof(QuadVertexData, Position));

		layout.AddLayoutFloat(4, sizeof(QuadVertexData), (const void*)offsetof(QuadVertexData, Color));

		layout.AddLayoutFloat(2, sizeof(QuadVertexData), (const void*)offsetof(QuadVertexData, TextureCoords));

		layout.AddLayoutFloat(1, sizeof(QuadVertexData), (const void*)offsetof(QuadVertexData, TextureIndex));

		uint32_t* indices = new uint32_t[QuadMaxIndexCount]{0};
		uint32_t offset = 0;

		for (int i = 0; i < QuadMaxIndexCount; i += 6)
		{
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;

			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}

		qptr->indexBuffer = IndexBuffer::CreateIndexBuffer(sizeof(uint32_t) * QuadMaxIndexCount, indices);
		delete[] indices;

		utils::SampleTextureOnShader(qptr->shader, 16, qptr->TexturesID);

		qptr->vertexArray->Unbind();

		//GLOBAL GL CONFIGS
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glEnable(GL_BLEND));
		GLCall(glEnable(GL_LINE_SMOOTH));
		//GLCall(glEnable(GL_MULTISAMPLE));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		return s_RenderInit;
	}

	void Render2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

		m_ShaderLib->Get("Quad")->Bind();
		m_ShaderLib->Get("Quad")->SetUniformMat4f("u_ViewProj", viewProj);

		s_Data.Quads->Stats.GeometryCounterPerFrame = 0;
	}

	void Render2D::Clear()
	{
		GLCommands::GL_Clear(GL_ClearCommand::ClearDepthColor);
	}

	void Render2D::BeginBatch()
	{
		s_Data.Quads->BufferPtr = s_Data.Quads->Buffer;
		s_Data.Quads->IndexCount = 0;
		s_Data.Quads->TextureSlotIndex = s_Data.Quads->WhiteTextureID;
	}

	void Render2D::EndBatch()
	{
		if (s_Data.Quads->IndexCount)
		{
			s_Data.Quads->vertexBuffer->Bind();
			s_Data.Quads->vertexBuffer->SubData(s_Data.Quads->SizePtr(),s_Data.Quads->Buffer);
			s_Data.Quads->vertexBuffer->Unbind();
		}
	}

	void Render2D::Flush()
	{
		if (s_Data.Quads->IndexCount)
		{
			s_Data.Quads->shader->Bind();

			for (int i = 0; i < s_Data.Quads->TextureSlotIndex; i++)
			{
				GLCall(glActiveTexture(GL_TEXTURE0 + i));
				GLCall(glBindTexture(GL_TEXTURE_2D, s_Data.Quads->TexturesID[i]));
			}

			s_Data.Quads->vertexArray->Bind();

			GLCall(glDrawElements(GL_TRIANGLES, s_Data.Quads->IndexCount, GL_UNSIGNED_INT, nullptr));

			s_Data.Quads->TextureSlotIndex = 1;
			s_Data.Quads->IndexCount = 0;
		}
	}

	void Render2D::Dispose()
	{
		using namespace utils;
		Delete(s_Data.Quads);
		Delete(m_ShaderLib.release());

		//s_Data.Quads->Dispose();

		Texture::DeleteWhiteTexture();
	}

	void Render2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		if (s_Data.Quads->IndexCount >= QuadMaxIndexCount)
		{
			EndBatch();
			Flush();
			BeginBatch();
		}

		QuadVertexData* qbuff = reinterpret_cast<QuadVertexData*>(s_Data.Quads->BufferPtr);
		uint8_t textureIndex = s_Data.Quads->TexturesID[s_Data.Quads->WhiteTextureID];
		for (size_t i = 0; i < 4; i++)
		{
			qbuff->Position = transform * QuadVertexPositions[i];
			qbuff->Color = color;
			qbuff->TextureCoords = DefaultTextureCoords[i];
			qbuff->TextureIndex = textureIndex;
			qbuff++;
		}

		s_Data.Quads->BufferPtr = reinterpret_cast<void*>(qbuff);

		s_Data.Quads->IndexCount += 6;
		s_Data.Quads->Stats.GeometryCounterPerFrame++;
		s_Data.Quads->Stats.GeometryCounterLifeSpam++;
	}

	void Render2D::DrawQuad(const glm::mat4& transform, Ref<Texture> texture, const glm::vec4& color)
	{
		if (s_Data.Quads->IndexCount >= QuadMaxIndexCount || s_Data.Quads->TextureSlotIndex >= 16-1)
		{
			EndBatch();
			Flush();
			BeginBatch();
		}

		uint8_t textureIndex = 0;

		if (texture)
		{
			for (uint8_t i = 1; i < s_Data.Quads->TextureSlotIndex; i++)
			{
				if (s_Data.Quads->TexturesID[i] == texture->GetId())
				{
					textureIndex = i;
					break;
				}
			}

			if (!textureIndex)
			{
				textureIndex = s_Data.Quads->TextureSlotIndex;
				s_Data.Quads->TexturesID[textureIndex] = texture->GetId();
				s_Data.Quads->TextureSlotIndex++;
			}
		}
		QuadVertexData* qbuff = reinterpret_cast<QuadVertexData*>(s_Data.Quads->BufferPtr);
		for (size_t i = 0; i < 4; i++)
		{
			qbuff->Position = transform * QuadVertexPositions[i];
			qbuff->Color = color;
			qbuff->TextureCoords = DefaultTextureCoords[i];
			qbuff->TextureIndex = textureIndex;
			qbuff++;
		}

		s_Data.Quads->IndexCount += 6;
		s_Data.Quads->Stats.GeometryCounterPerFrame++;
		s_Data.Quads->Stats.GeometryCounterLifeSpam++;
	}

	RenderStats Render2D::GetRenderInfo(const DrawInfo& geometry)
	{
		switch (geometry)
		{
		case DrawInfo::Quad:
			return s_Data.Quads->Stats;
		}
	}

}