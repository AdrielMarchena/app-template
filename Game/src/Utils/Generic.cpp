#include "pch.h"
#include "Generic.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Game
{
namespace utils
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
}