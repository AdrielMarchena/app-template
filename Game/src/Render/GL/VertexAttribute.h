#pragma once
#include "VertexArray.h"
#include "VertexBuffer.h"

namespace Game
{
	class VertexAttribute
	{
		Ref<VertexBuffer> Vb;
		uint32_t current_index = 0;
	public:
		VertexAttribute(Ref<VertexBuffer>& vb)
			:Vb(vb) {}

		void AddLayoutFloat(int32_t size, uint32_t stride,const void* pointer);
		void AddLayoutUint(int32_t size, uint32_t stride,const void* pointer);
		void AddLayoutInt(int32_t size, uint32_t stride,const void* pointer);
	};
}