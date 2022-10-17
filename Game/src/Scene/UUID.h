#pragma once
#include <stdint.h>
#ifdef GAME_WINDOWS_BUILD

#include <xhash>

#else

#include <bits/functional_hash.h>

#endif
#include <unordered_map>

namespace Game
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_Uuid; }
		bool operator==(const UUID& other) { return this->m_Uuid == other.m_Uuid; }
	private:
		uint64_t m_Uuid = NULL;
	};
}


//This make possible to use UUID as a key in a unordered_map for example
namespace std
{
	template<>
	struct hash<Game::UUID>
	{
		std::size_t operator()(const Game::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}