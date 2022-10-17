#pragma once

#include <string>
#include "glm/glm.hpp"
namespace Game
{
namespace utils
{
	template<typename T>
	using Predicate = std::function<bool(T)>;

	std::string ToUpper(const std::string& str);
	std::string ToLower(const std::string& str);

	std::vector<std::string> GetFilesFromFolder(const std::string& folder);
	std::string ParseFileName(const std::string& path);
	glm::mat4 pos_trans(const glm::vec3& pos, const glm::vec2& size);

	template<typename T>
	bool Any(std::vector<T> collection, Predicate<T> predicate)
	{
		for (const auto& t : collection)
			if(predicate(t)) return true;
		return false;
	}

	template<typename T, typename _Map>
	_Map::iterator Find(_Map collection, Predicate<T> predicate)
	{
		for (auto it = collection.begin(); it != collection.end(); it++)
		{
			if(predicate((*it).first));
			return it;
		}
		return collection.end();
	}

	template<typename Key, typename T>
	std::vector<T> GetValues(std::unordered_map<Key, T>& umap)
	{
		std::vector<T> vec(umap.size());
		for (auto& v : umap)
			vec.emplace_back(v);
		return vec;
	}
}
}