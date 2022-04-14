#pragma once
#include "Message.h"
#include "Debug/Intrumentator.h"

#include <queue>
#include <vector>
#include <functional>
namespace Game
{
	using MessageReceiverFunction = std::function<void(Game::Message)>;
	class MessageBus
	{
	public:
		MessageBus() = default;
		~MessageBus() = default;

		int AddReceiver(MessageReceiverFunction messageReceiver, size_t p_index = -1)
		{
			GAME_PROFILE_FUNCTION();
			//TODO: Change this in the future, but for now,
			// Nothing will be removed from this vector, just set to nullptr on that index

			size_t index = p_index;
			constexpr size_t wrapped_arround_size_t = -1; // This is really large

			if (index == wrapped_arround_size_t) // check if busNode have a valid index
			{
				for (int i = 0; i < m_Receivers.size(); i++)//Search for a nullptr slot, wich are set this way when a busNode is deleted
				{
					if (m_Receivers[i] == nullptr) // if find a nullptr, use this slot
					{
						index = i;
						m_Receivers[i] = messageReceiver;
						return index;
					}
				}
				 //If any nullptr slot is find, create a new one
				index = m_Receivers.size();
				m_Receivers.push_back(messageReceiver);
				return index;
			}

			if(m_Receivers.size() < index && index != wrapped_arround_size_t) // To override a function on your on slot
				m_Receivers[index] = messageReceiver;

			return index;
		}

		void AddMessage(Game::Message message)
		{
			m_MessagesQueue.push(message);
		}

		void Notify()
		{
			GAME_PROFILE_FUNCTION();
			while (!m_MessagesQueue.empty())
			{
				for (auto iter = m_Receivers.begin(); iter != m_Receivers.end(); iter++)
				{
					auto& func = *iter;
					if (!func)
						continue;
					func(m_MessagesQueue.front());
				}
				m_MessagesQueue.pop();
			}
		}

	private:
		std::vector<MessageReceiverFunction> m_Receivers;
		std::queue<Game::Message> m_MessagesQueue;
	};
}


