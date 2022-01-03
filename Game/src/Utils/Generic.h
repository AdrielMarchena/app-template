#pragma once

#include <string>
#include "glm/glm.hpp"
namespace Game
{
namespace utils
{
	std::string ToUpper(const std::string& str);
	std::string ToLower(const std::string& str);

	glm::mat4 pos_trans(const glm::vec3& pos, const glm::vec2& size);
}
}