/*
*	@file Keyboard.cpp
*	@author Adriel Marchena Santos
*	
*	Keyboard stuff
*/
#include "pch.h"
#include "Keyboard.h"
#include <iostream>
#include "Log/Log.h"

namespace Game
{
	bool Keyboard::AnyKey = false;
	std::array<bool, EN_KEYBOARD_NUMBER_KEYS> Keyboard::keysPressed{false};
	std::array<bool, EN_KEYBOARD_NUMBER_KEYS> Keyboard::singleClickControl{false};
	
	void Keyboard::Init()
	{
		//TODO: Idk, remove latter i guess
	}

	bool Keyboard::isPress(int32_t key)
	{
		key -= 32;
		return keysPressed[key];
	}
	bool Keyboard::isClicked(int32_t key)
	{
		key -= 32;
		if (!keysPressed[key] || singleClickControl[key])
			return false;
		
		singleClickControl[key] = true;
		return keysPressed[key];
	}

	bool Keyboard::isAnyKey()
	{
		return AnyKey;
	}

	void Keyboard::on_keyboard_button(int32_t key, int32_t scancode, int32_t action, int32_t mods)
	{
		key -= 32;
		switch (action)
		{
		case GAME_PRESS:
			keysPressed[key] = true;
			AnyKey = true;
			break;
		case GAME_BUTTON_RELEASE:
			keysPressed[key] = false;
			singleClickControl[key] = false;
			AnyKey = false;
			break;
		case GAME_REPEAT:
			break;
		default:
			GAME_LOG_WARN("Unknow Keyboard Action '{0}'!", action);
			//std::cout << "Unknow Keyboard Action '" << action << "'!" << std::endl;
		}
	}
}
