#include "pch.h"
#include "Generic.h"

#include "glm/gtc/matrix_transform.hpp"
#include <filesystem>

namespace Game
{
std::string ToUpper(const std::string& str)
{
	std::string tmp = "";
	for (auto& c : str)
	{
		if (c >= 'a' && c <= 'z')
			tmp += c - 32;
		else
			tmp += c;
	}
	return tmp;
}

std::string ToLower(const std::string& str)
{
	std::string tmp = "";
	for (auto& c : str)
	{
		if (c >= 'A' && c <= 'Z')
			tmp += c + 32;
		else
			tmp += c;
	}
	return tmp;
}

std::vector<std::string> GetFilesFromFolder(const std::string& folder)
{
	using std::filesystem::directory_iterator;
	std::vector<std::string> paths;
	for (const auto& file : directory_iterator(folder))
		paths.push_back(file.path().string());
	return paths;
}

std::string ParseFileName(const std::string& path)
{
	auto lastSlash = path.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	auto lastDot = path.rfind('.');
	auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
	return path.substr(lastSlash, count);
}

glm::mat4 pos_trans(const glm::vec3& pos, const glm::vec2& size)
{
	return glm::translate(glm::mat4(1.0f), pos)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
}
}