/*
* @file Files.h
* @author Adriel Marchena Santos
*
* File handlind things here
*/

#pragma once

#include <vector>
#include <string>

//#include "base_exceptions.h"

namespace Game
{
namespace utils
{
	class Files
	{
	public:
		Files(){}

		static std::vector<std::pair<std::string, std::string>> GetPairText(const std::string& dir, const std::string& ext = ".jpg#.jpeg#.png#.PNG");
	};
}
}

