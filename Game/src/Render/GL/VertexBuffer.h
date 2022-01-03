#pragma once
#include <stdint.h>
#include "Core/Core.h"
#include "GL_Types.h"
namespace Game
{
	class VertexBuffer
	{
	private:
		uint32_t m_Id = 0;
		GL_Usage m_Usage = GL_Usage::DYNAMIC;
	public:
		VertexBuffer() = default;
		~VertexBuffer();

		void Bind();
		void Unbind();
		void Dispose();

		void SubData(size_t size, const void* buffer, int offset = 0);

		static Ref<VertexBuffer> CreateVertexBuffer(size_t size,GL_Usage usage = GL_Usage::DYNAMIC);
	};
}