#pragma once
#include <string>
#include "Scene/Entity.h"
namespace Game
{
	struct MessageEvent
	{
		enum : uint32_t
		{
			//Add your events here
			MESSAGE_COLISION = 0,
			PLAYER_DIED = 1,

		};
		MessageEvent(uint32_t type):TypeID(type){}
		uint32_t TypeID;
		operator uint32_t() const { return TypeID; }
		operator uint32_t&() { return TypeID; }
	};

	class Message
	{
	public:
		Message(Entity* entity, MessageEvent event);

		MessageEvent GetEvent() const { return m_MessageEvent; }
		Entity GetEntity();
		bool EntityIsValid() const;
	private:
		Entity* m_OtherEntity;
		MessageEvent m_MessageEvent;
	};


}

