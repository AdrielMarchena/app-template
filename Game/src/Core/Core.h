#pragma once

#include <memory>
#include "Assert.h"
namespace Game
{
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, class ... _Args>
	Ref<T> MakeRef(_Args&&... args)
	{
		return std::make_shared<T>(std::forward<_Args>(args)...);
	}

	template<typename T>
	using Weak = std::weak_ptr<T>;

	template<typename T>
	Weak<T> MakeWeak(const Ref<T>& ref)
	{
		return ref;
	}

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, class ... _Args>
	Scope<T> MakeScope(_Args&&... args)
	{
		return std::make_unique<T>(std::forward<_Args>(args)...);
	}

}