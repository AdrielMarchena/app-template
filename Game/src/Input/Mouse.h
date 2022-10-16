/*
*	@file Mouse.h
*	@author Adriel Marchena Santos
*	
*	Mouse stuff
*
*/

#pragma once
#include "Core/Core.h"

#include <array>
#include "glm/glm.hpp"
#include "MouseKeys.h"

#include <algorithm>

constexpr unsigned char EN_MOUSE_NUMBER_KEYS = 32;
namespace Game
{
	class Mouse
	{
	private:
		static std::array<bool, EN_MOUSE_NUMBER_KEYS> keysPressed;
		static std::array<bool, EN_MOUSE_NUMBER_KEYS> singleClickControl;

		static glm::vec2 pos;
		static glm::vec2 prevPos;
		static glm::vec2 OffValue;
		static glm::vec2 PrevOff;
		static bool AnyKey;
	public:
		~Mouse(){}

		static void Init();

		static bool isClicked(int32_t key);
		static bool isPress(int32_t key);
		
		static bool isAnyKey();

		static void on_mouse_button(int32_t key, int32_t action, int32_t mods);
		static void on_mouse_scroll(double_t xOffSet, double_t yOffSet);
		static void on_mouse_cursor(double_t xPos, double_t yPos);

		static glm::vec2 gpos() { return pos; }
		static glm::vec2 gprevPos()  { return prevPos; }
		static glm::vec2 goffValue()
		{ 
			auto aux = OffValue;
			OffValue = { 0.0f,0.0f };
			return aux;
		}
		static glm::vec2 gprevOffValue() { return PrevOff; }

		//static glm::vec2 m_pos() { return { pos.x, -(pos.y - WindowProps().height) }; }
		static glm::vec2 m_pos(int h) { return { pos.x, -(pos.y - h) }; }
		static glm::vec2 screen_pos(float w, float h, float camera_size)
		{
			float ar = (float)w / (float)h;

			float devX = pos.x / ((w) * 0.5f) - 1.0f;
			float devY = -1.0f * (pos.y / (h * 0.5f) - 1.0f);

			float csize = camera_size * 0.5f;

			return { devX * csize * ar, devY * csize };
		}

		static void clamp_cursor(float_t min_x, float_t max_x, float_t min_y, float_t max_y);
	};
}


