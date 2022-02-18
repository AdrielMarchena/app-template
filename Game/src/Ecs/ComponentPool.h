#pragma once

#include "General.h"
#include <stdint.h>

namespace ecs
{
	struct ComponentPool
	{
		char* pData{ nullptr };
		size_t elementSize{0};

		ComponentPool() = default;

		operator bool() const { return pData != nullptr; }

		ComponentPool(size_t element_size)
			:elementSize(element_size)
		{
			pData = new char[elementSize * MAX_COMPONENTS];
		}

		~ComponentPool()
		{
			if(pData)
				delete[] pData;
		}

		inline void* get(size_t index)
		{
			return pData + index * elementSize;
		}

	};
}