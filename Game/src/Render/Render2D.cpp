#include "pch.h"
#include "Render2D.h"

#include "Debug/Intrumentator.h"
#include "GL/GL_Include.h"
#include "GL/gl_error_macro_db.h"
#include "glad/glad.h"

#include "GL/GLContext.h"

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
			GAME_PROFILE_FUNCTION();
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

	static int32_t CircleMaxCount = 500;
	static int32_t CircleMaxVertexCount = CircleMaxCount * 4;
	static int32_t CircleMaxIndexCount = CircleMaxCount * 6;

	static int32_t LineMaxCount = 500;
	static int32_t LineMaxVertexCount = LineMaxCount * 2;
	static int32_t LineMaxIndexCount = LineMaxCount * 2;

	static int32_t MaxTextureUnits = 8;

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

		GAME_DECLARE_ENTITY_ID;
	};

	struct CircleVertexData
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoords;
		float TextureIndex;

		glm::vec3 LocalPosition;
		float Thick;
		float Fade;

		GAME_DECLARE_ENTITY_ID;
	};

	struct LineVertexData
	{
		glm::vec3 Position;
		glm::vec4 Color;

		GAME_DECLARE_ENTITY_ID;
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
			GAME_PROFILE_FUNCTION();
			//Temp
			vertexArray->Dispose();
			vertexBuffer->Dispose();
			indexBuffer->Dispose();
			TexturesID.clear();
			shader.~shared_ptr();
		}
	};

	// All info about a Geometry are here
	struct DrawDataCollection
	{
		DrawGeometryData* GeometryData = nullptr;
	};

	static DrawDataCollection s_QuadData;
	static DrawDataCollection s_CircleData;
	static DrawDataCollection s_LineData;

	bool Render2D::Init()
	{
		GAME_PROFILE_FUNCTION();
		if (s_RenderInit)
			return s_RenderInit;

		s_RenderInit = true;

		MaxTextureUnits = GLContext::GetFragmetShaderMaxTextureUnits();

		//Shader is managed as a Ref(shared_ptr) inside ShaderLib
		m_ShaderLib = MakeScope<ShaderLib>();

		{ //Quad stuff
			auto quadShader = Shader::CreateShader("assets/shaders/Quad.glsl");
			m_ShaderLib->Add(quadShader);

			//Create Quad GL stuff
			s_QuadData.GeometryData = new DrawGeometryData();
			auto qptr = s_QuadData.GeometryData; /* Handler pointer to a easier name */

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

			GAME_DO_IF_ENTITYID(layout.AddLayoutInt(1, sizeof(QuadVertexData), (const void*)offsetof(QuadVertexData, GAME_ENTITY_ID)));

			uint32_t* indices = new uint32_t[QuadMaxIndexCount]{ 0 };
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

			utils::SampleTextureOnShader(qptr->shader, MaxTextureUnits, qptr->TexturesID);
			qptr->vertexArray->Unbind();
		}
		// Circle Stuff
		{
			auto circleShader = Shader::CreateShader("assets/shaders/Circle.glsl");
			m_ShaderLib->Add(circleShader);

			s_CircleData.GeometryData = new DrawGeometryData();
			auto cptr = s_CircleData.GeometryData;

			cptr->shader = circleShader;
			cptr->shader->Bind();

			cptr->Buffer = new QuadVertexData[CircleMaxVertexCount];
			cptr->BufferPtr = cptr->Buffer;

			cptr->vertexArray = VertexArray::CreateVertexArray();

			cptr->vertexBuffer = VertexBuffer::CreateVertexBuffer(CircleMaxVertexCount * sizeof(CircleVertexData));

			VertexAttribute layout(cptr->vertexBuffer);

			layout.AddLayoutFloat(3, sizeof(CircleVertexData), (const void*)offsetof(CircleVertexData, Position));

			layout.AddLayoutFloat(4, sizeof(CircleVertexData), (const void*)offsetof(CircleVertexData, Color));

			layout.AddLayoutFloat(2, sizeof(CircleVertexData), (const void*)offsetof(CircleVertexData, TextureCoords));

			layout.AddLayoutFloat(1, sizeof(CircleVertexData), (const void*)offsetof(CircleVertexData, TextureIndex));

			layout.AddLayoutFloat(3, sizeof(CircleVertexData), (const void*)offsetof(CircleVertexData, LocalPosition));

			layout.AddLayoutFloat(1, sizeof(CircleVertexData), (const void*)offsetof(CircleVertexData, Thick));
			
			layout.AddLayoutFloat(1, sizeof(CircleVertexData), (const void*)offsetof(CircleVertexData, Fade));

			GAME_DO_IF_ENTITYID(layout.AddLayoutInt(1, sizeof(CircleVertexData), (const void*)offsetof(CircleVertexData, GAME_ENTITY_ID)));

			if (CircleMaxIndexCount <= QuadMaxIndexCount) // Reuse Quad Index buffer if circle index buffer max size is not greater than the quad one
			{
				cptr->indexBuffer = s_QuadData.GeometryData->indexBuffer;
				cptr->indexBuffer->Bind();

			}
			else
			{
				uint32_t* indices = new uint32_t[CircleMaxIndexCount]{ 0 };
				uint32_t offset = 0;

				for (int i = 0; i < CircleMaxIndexCount; i += 6)
				{
					indices[i + 0] = 0 + offset;
					indices[i + 1] = 1 + offset;
					indices[i + 2] = 2 + offset;

					indices[i + 3] = 2 + offset;
					indices[i + 4] = 3 + offset;
					indices[i + 5] = 0 + offset;

					offset += 4;
				}

				cptr->indexBuffer = IndexBuffer::CreateIndexBuffer(sizeof(uint32_t) * CircleMaxIndexCount, indices);
				delete[] indices;
			}

			utils::SampleTextureOnShader(cptr->shader, MaxTextureUnits, cptr->TexturesID);
			cptr->vertexArray->Unbind();
		}

		//Lines
		{
			auto lineShader = Shader::CreateShader("assets/shaders/Line.glsl");
			m_ShaderLib->Add(lineShader);

			s_LineData.GeometryData = new DrawGeometryData();
			auto cptr = s_LineData.GeometryData;

			cptr->Buffer = new LineVertexData[LineMaxVertexCount];
			cptr->BufferPtr = cptr->Buffer;

			cptr->shader = lineShader;
			cptr->shader->Bind();

			cptr->vertexArray = VertexArray::CreateVertexArray();

			cptr->vertexBuffer = VertexBuffer::CreateVertexBuffer(LineMaxVertexCount * sizeof(LineVertexData));

			VertexAttribute line_layout(cptr->vertexBuffer);
			line_layout.AddLayoutFloat(3, sizeof(LineVertexData), (const void*)offsetof(LineVertexData, Position));

			line_layout.AddLayoutFloat(4, sizeof(LineVertexData), (const void*)offsetof(LineVertexData, Color));

			line_layout.AddLayoutInt(1, sizeof(LineVertexData), (const void*)offsetof(LineVertexData, entityID));

			cptr->vertexArray->Unbind();
		}


		//GLOBAL GL CONFIGS
		Enable({
			GLEnableCaps::DEPTH_TEST,
			GLEnableCaps::BLEND,
			GLEnableCaps::LINE_SMOOTH
		});
		SetBlendFunc(GLBlendFactor::SRC_ALPHA, GLBlendFactor::ONE_MINUS_SRC_ALPHA);

		return s_RenderInit;
	}

	void Render2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		GAME_PROFILE_FUNCTION();
		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

		m_ShaderLib->Get("Quad")->Bind();
		m_ShaderLib->Get("Quad")->SetUniformMat4f("u_ViewProj", viewProj);

		m_ShaderLib->Get("Circle")->Bind();
		m_ShaderLib->Get("Circle")->SetUniformMat4f("u_ViewProj", viewProj);

		m_ShaderLib->Get("Line")->Bind();
		m_ShaderLib->Get("Line")->SetUniformMat4f("u_ViewProj", viewProj);

		s_QuadData.GeometryData->Stats.GeometryCounterPerFrame = 0;
		s_CircleData.GeometryData->Stats.GeometryCounterPerFrame = 0;
		s_LineData.GeometryData->Stats.GeometryCounterPerFrame = 0;
	}

	void Render2D::Clear()
	{
		GAME_PROFILE_FUNCTION();
		GLCommands::GL_Clear(GL_ClearCommand::ClearDepthColor);
	}

	void Render2D::ClearNoDepth()
	{
		GAME_PROFILE_FUNCTION();
		GLCommands::GL_Clear(GL_ClearCommand::ClearColor);
	}

	static inline void ResetGeometryData(DrawGeometryData* data)
	{
		data->BufferPtr = data->Buffer;
		data->IndexCount = 0;
		data->TextureSlotIndex = data->WhiteTextureID;
	}

	void Render2D::BeginBatch()
	{
		GAME_PROFILE_FUNCTION();
		ResetGeometryData(s_QuadData.GeometryData);
		ResetGeometryData(s_CircleData.GeometryData);
		ResetGeometryData(s_LineData.GeometryData);
	}

	static inline void EndGeometryData(DrawGeometryData* data)
	{
		if (!data->IndexCount)
			return;

		data->vertexBuffer->Bind();
		data->vertexBuffer->SubData(data->SizePtr(), data->Buffer);
		data->vertexBuffer->Unbind();
	}

	void Render2D::EndBatch()
	{
		GAME_PROFILE_FUNCTION();
		EndGeometryData(s_QuadData.GeometryData);
		EndGeometryData(s_CircleData.GeometryData);
		EndGeometryData(s_LineData.GeometryData);
	}

	static inline void DrawTexturedGeometry(DrawGeometryData* data)
	{
		if (!data->IndexCount)
			return;

		data->shader->Bind();

		for (int i = 0; i < data->TextureSlotIndex; i++)
		{
			GLCall(glActiveTexture(GL_TEXTURE0 + i));
			GLCall(glBindTexture(GL_TEXTURE_2D, data->TexturesID[i]));
		}

		data->vertexArray->Bind();

		GLCall(glDrawElements(GL_TRIANGLES, data->IndexCount, GL_UNSIGNED_INT, nullptr));

		data->TextureSlotIndex = 1;
		data->IndexCount = 0;
	}

	static inline void DrawGeometry(DrawGeometryData* data)
	{
		if (!data->IndexCount)
			return;

		data->shader->Bind();
		data->vertexArray->Bind();

		GLCall(glDrawArrays(GL_LINES, 0, data->IndexCount));
		data->IndexCount = 0;
	}

	void Render2D::Flush()
	{
		GAME_PROFILE_FUNCTION();

		DrawGeometry(s_LineData.GeometryData);
		DrawTexturedGeometry(s_QuadData.GeometryData);
		DrawTexturedGeometry(s_CircleData.GeometryData);
	}

	void Render2D::Dispose()
	{
		GAME_PROFILE_FUNCTION();
		using namespace utils;
		Delete(s_QuadData.GeometryData);
		Delete(s_CircleData.GeometryData);
		Delete(s_LineData.GeometryData);
		Delete(m_ShaderLib.release());

		//s_Data.GeometryData->Dispose();

		Texture::DeleteWhiteTexture();
	}

	void Render2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color GAME_COMMA_ENTITYID(GAME_DECLARE_ENTITY_ID))
	{
		GAME_PROFILE_FUNCTION();
		if (s_QuadData.GeometryData->IndexCount >= QuadMaxIndexCount)
		{
			EndBatch();
			Flush();
			BeginBatch();
		}

		QuadVertexData* qbuff = reinterpret_cast<QuadVertexData*>(s_QuadData.GeometryData->BufferPtr);
		uint8_t textureIndex = s_QuadData.GeometryData->TexturesID[s_QuadData.GeometryData->WhiteTextureID];
		for (size_t i = 0; i < 4; i++)
		{
			qbuff->Position = transform * QuadVertexPositions[i];
			qbuff->Color = color;
			qbuff->TextureCoords = DefaultTextureCoords[i];
			qbuff->TextureIndex = textureIndex;
			qbuff++;
		}

		s_QuadData.GeometryData->BufferPtr = reinterpret_cast<void*>(qbuff);

		s_QuadData.GeometryData->IndexCount += 6;
		s_QuadData.GeometryData->Stats.GeometryCounterPerFrame++;
		s_QuadData.GeometryData->Stats.GeometryCounterLifeSpam++;
	}

	void Render2D::DrawQuad(const glm::mat4& transform, Ref<Texture> texture, const glm::vec4& color GAME_COMMA_ENTITYID(GAME_DECLARE_ENTITY_ID))
	{
		GAME_PROFILE_FUNCTION();
		if (s_QuadData.GeometryData->IndexCount >= QuadMaxIndexCount || s_QuadData.GeometryData->TextureSlotIndex >= 16-1)
		{
			EndBatch();
			Flush();
			BeginBatch();
		}

		uint8_t textureIndex = 0;

		if (texture)
		{
			for (uint8_t i = 1; i < s_QuadData.GeometryData->TextureSlotIndex; i++)
			{
				if (s_QuadData.GeometryData->TexturesID[i] == texture->GetId())
				{
					textureIndex = i;
					break;
				}
			}

			if (!textureIndex)
			{
				textureIndex = s_QuadData.GeometryData->TextureSlotIndex;
				s_QuadData.GeometryData->TexturesID[textureIndex] = texture->GetId();
				s_QuadData.GeometryData->TextureSlotIndex++;
			}
		}
		QuadVertexData* qbuff = reinterpret_cast<QuadVertexData*>(s_QuadData.GeometryData->BufferPtr);
		for (size_t i = 0; i < 4; i++)
		{
			qbuff->Position = transform * QuadVertexPositions[i];
			qbuff->Color = color;
			qbuff->TextureCoords = DefaultTextureCoords[i];
			qbuff->TextureIndex = textureIndex;
			GAME_DO_IF_ENTITYID(qbuff->GAME_ENTITY_ID = GAME_ENTITY_ID);
			qbuff++;
		}

		s_QuadData.GeometryData->BufferPtr = reinterpret_cast<void*>(qbuff);

		s_QuadData.GeometryData->IndexCount += 6;
		s_QuadData.GeometryData->Stats.GeometryCounterPerFrame++;
		s_QuadData.GeometryData->Stats.GeometryCounterLifeSpam++;
	}

	void Render2D::DrawCircle(const glm::mat4& transform, float thick, float fade, const glm::vec4& color GAME_COMMA_ENTITYID(GAME_DECLARE_ENTITY_ID))
	{
		if (s_CircleData.GeometryData->IndexCount >= CircleMaxIndexCount)
		{
			EndBatch();
			Flush();
			BeginBatch();
		}

		CircleVertexData* cbuff = reinterpret_cast<CircleVertexData*>(s_CircleData.GeometryData->BufferPtr);
		uint8_t textureIndex = s_CircleData.GeometryData->TexturesID[s_CircleData.GeometryData->WhiteTextureID];
		for (uint8_t i = 0; i < 4; i++)
		{
			cbuff->Position = transform * QuadVertexPositions[i];
			cbuff->Color = color;
			cbuff->TextureCoords = DefaultTextureCoords[i];
			cbuff->TextureIndex = textureIndex;
			cbuff->LocalPosition = QuadVertexPositions[i] * 2.0f;
			cbuff->Thick = thick;
			cbuff->Fade = fade;
			cbuff->entityID = entityID;
			cbuff++;
		}

		s_CircleData.GeometryData->BufferPtr = reinterpret_cast<void*>(cbuff);

		s_CircleData.GeometryData->IndexCount += 6;
		s_CircleData.GeometryData->Stats.GeometryCounterPerFrame++;
		s_CircleData.GeometryData->Stats.GeometryCounterLifeSpam++;
	}

	void Render2D::DrawCircle(const glm::mat4& transform, float thick, float fade, Ref<Texture> texture, const glm::vec4& color GAME_COMMA_ENTITYID(GAME_DECLARE_ENTITY_ID))
	{
		if (s_CircleData.GeometryData->IndexCount >= CircleMaxIndexCount || s_CircleData.GeometryData->TextureSlotIndex > MaxTextureUnits - 1)
		{
			EndBatch();
			Flush();
			BeginBatch();
		}

		uint8_t textureIndex = 0;

		if (texture)
		{
			for (uint8_t i = 1; i < s_QuadData.GeometryData->TextureSlotIndex; i++)
			{
				if (s_QuadData.GeometryData->TexturesID[i] == texture->GetId())
				{
					textureIndex = i;
					break;
				}
			}

			if (!textureIndex)
			{
				textureIndex = s_QuadData.GeometryData->TextureSlotIndex;
				s_QuadData.GeometryData->TexturesID[textureIndex] = texture->GetId();
				s_QuadData.GeometryData->TextureSlotIndex++;
			}
		}
		CircleVertexData* cbuff = reinterpret_cast<CircleVertexData*>(s_CircleData.GeometryData->BufferPtr);
		for (uint8_t i = 0; i < 4; i++)
		{
			cbuff->Position = transform * QuadVertexPositions[i];
			cbuff->Color = color;
			cbuff->TextureCoords = DefaultTextureCoords[i];
			cbuff->TextureIndex = textureIndex;
			cbuff->LocalPosition = QuadVertexPositions[i] * 2.0f;
			cbuff->Thick = thick;
			cbuff->Fade = fade;
			cbuff->entityID = entityID;
			cbuff++;
		}

		s_CircleData.GeometryData->BufferPtr = reinterpret_cast<void*>(cbuff);

		s_CircleData.GeometryData->IndexCount += 6;
		s_CircleData.GeometryData->Stats.GeometryCounterPerFrame++;
		s_CircleData.GeometryData->Stats.GeometryCounterLifeSpam++;
	}

	void Render2D::DrawLine(const glm::vec3& origin, glm::vec3& dest, const glm::vec4& color GAME_COMMA_ENTITYID(GAME_DECLARE_ENTITY_ID))
	{
		if (s_LineData.GeometryData->IndexCount >= LineMaxIndexCount)
		{
			EndBatch();
			Flush();
			BeginBatch();
		}

		LineVertexData* cbuff = reinterpret_cast<LineVertexData*>(s_LineData.GeometryData->BufferPtr);

		cbuff->Position = origin;
		cbuff->Color = color;
		cbuff->entityID = entityID;
		cbuff++;

		cbuff->Position = dest;
		cbuff->Color = color;
		cbuff->entityID = entityID;
		cbuff++;

		s_LineData.GeometryData->BufferPtr = reinterpret_cast<void*>(cbuff);

		s_LineData.GeometryData->IndexCount += 2;
		s_LineData.GeometryData->Stats.GeometryCounterPerFrame++;
		s_LineData.GeometryData->Stats.GeometryCounterLifeSpam++;
	}

	RenderStats Render2D::GetRenderInfo(const DrawInfo& geometry)
	{
		switch (geometry)
		{
			case DrawInfo::Quad:
				return s_QuadData.GeometryData->Stats;
			case DrawInfo::Circle:
				return s_CircleData.GeometryData->Stats;
		}
	}

	void Render2D::Enable(std::vector<GLEnableCaps> cap)
	{
		for (auto c : cap)
		{
			GLCall(glEnable(GLEnable::GetGlEnableCaps(c)));
		}
	}

	void Render2D::Disable(std::vector<GLEnableCaps> cap)
	{
		for (auto c : cap)
		{
			GLCall(glDisable(GLEnable::GetGlEnableCaps(c)));
		}
	}

	void Render2D::SetBlendFunc(GLBlendFactor sfactor, GLBlendFactor dfactor)
	{
		GLCall(glBlendFunc(
				GLBlend::GetGLBlend(sfactor),
				GLBlend::GetGLBlend(dfactor)
			)
		);
	}
}
