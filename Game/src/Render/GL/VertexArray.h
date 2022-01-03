#pragma once
#include <stdint.h>
#include "Core/Core.h"
namespace Game
{
	class VertexArray
	{
	private:
		uint32_t m_Id = 0;
	public:
		VertexArray() = default;
		~VertexArray();

		void Bind();
		void Unbind();
		void Dispose();

		static Ref<VertexArray> CreateVertexArray();
	};
}