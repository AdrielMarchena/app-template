#pragma once

#include "Message.h"
#include "MessageBus.h"

#include "Log/Log.h"

#include <iostream>
#include <functional>
namespace Game
{
	/* Class Created inside a MessagerComponent, and created by the Scene class */
	class BusNode
	{
	public:
		BusNode(MessageBus* messageBus, MessageReceiverFunction receiver = nullptr)
			:m_MessageBus(messageBus),m_MessageNotifyFunc(receiver)
		{
			if (m_MessageNotifyFunc == nullptr)
				m_MessageNotifyFunc = std::bind(&BusNode::DefaultOnNotifyFunction, this, std::placeholders::_1);
			m_Index = m_MessageBus->AddReceiver(std::bind(&BusNode::OnNotify, this, std::placeholders::_1), m_Index);
		}

		~BusNode()
		{
			m_MessageBus->AddReceiver(nullptr, m_Index);
		}

	public:

		void Send(Message message)
		{
			m_MessageBus->AddMessage(message);
		}

		void OnNotify(Message message)
		{
			if (!m_MessageNotifyFunc)
			{
				GAME_LOG_WARN("There is no binded 'MessageReceiverFunction', please, provide one by calling 'BindNotifyFuntion method'");
			}
			else
				m_MessageNotifyFunc(message);
		}

		void BindNotifyFunction(MessageReceiverFunction notifyFunction)
		{
			//TODO: The previous one is not removed from the bus
			m_MessageNotifyFunc = notifyFunction;
			//m_Index = m_MessageBus->AddReceiver(m_MessageNotifyFunc,m_Index);
		}

	private:
		void DefaultOnNotifyFunction(Message message)
		{
			GAME_LOG_INFO("Using default function after receiving message: Entity {0} | Type(Event) {1}", message.GetEntity().GetHandler(), message.GetEvent());
		}

		void NotifyInternal(Message message)
		{
			this->OnNotify(message);
		}
		MessageBus* m_MessageBus = nullptr;
		MessageReceiverFunction m_MessageNotifyFunc;
		size_t m_Index = -1;
	};

	struct MessageComponent
	{
		BusNode Node;

		MessageComponent(MessageBus* bus):Node(bus){}
		MessageComponent(const MessageComponent&) = default;
	};
}

