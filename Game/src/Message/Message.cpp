#include "pch.h"
#include "Message.h"

namespace Game
{
	Message::Message(Entity* entity, MessageEvent event)
		:m_OtherEntity(entity), m_MessageEvent(event)
	{
	}
	Entity Message::GetEntity()
	{
		return *m_OtherEntity;
	}
	bool Message::EntityIsValid() const
	{
		return m_OtherEntity;
	}
}