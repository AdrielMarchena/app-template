/*
*	@file Keyboard.h
*	@author Adriel Marchena Santos
*	
*	Keyboard stuff
*/

#pragma once
#include <array>
#include "glm/glm.hpp"
#include "KeyboardKeys.h"

constexpr unsigned short EN_KEYBOARD_NUMBER_KEYS = 348*2; // Bizarre numbers there
namespace Game
{
	class Keyboard
	{
	private:
		static std::array<bool, EN_KEYBOARD_NUMBER_KEYS> keysPressed;
		static std::array<bool, EN_KEYBOARD_NUMBER_KEYS> singleClickControl;
		static bool AnyKey;
	public:

		static void Init();

		static bool isClicked(int32_t key);
		static bool isPress(int32_t key);

		static bool isAnyKey();

		static void on_keyboard_button(int32_t key, int32_t scancode, int32_t action, int32_t mods);
	};
}
