#pragma once
#include "Core/Core.h"
#include "Entry/Timestamp.h"
namespace Game
{
	struct ScriptMessage
	{
		enum class Type
		{
			Colision,

		};
	};

	class ScriptableClass
	{
	public:
		ScriptableClass() = default;
		virtual ~ScriptableClass() = default;

protected:
		virtual void OnAttach(){}
		virtual void OnAwake(){}
		virtual void OnUpdate(Timestamp dt){}
		virtual void OnDestroy(){}
private:
	friend class Scene;
	};
}