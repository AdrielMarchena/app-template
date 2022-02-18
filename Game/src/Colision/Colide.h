#pragma once
#include "glm/glm.hpp"
#include <algorithm>
namespace Game
{
	struct ColisionRect
	{
		glm::vec2 pos;
		glm::vec2 size;
		glm::vec2 velocity;

		ColisionRect& operator<<(const ColisionRect& right)
		{
			this->pos = (right.pos + (right.size / 2.0f) - (this->size / 2.0f));
			return *this;
		}
	};

	class Colide
	{
	public:
		_NODISCARD static bool RectVsRect(const ColisionRect& t, const ColisionRect& f);

		_NODISCARD static bool PointVsRect(const glm::vec2& p, const ColisionRect& r);

		_NODISCARD static bool RayVsRect(const glm::vec2& ray_origin, const glm::vec2& ray_dir, const ColisionRect& target, glm::vec2& contact_point, glm::vec2& contact_normal, float_t& t_hit_near);

		_NODISCARD static bool DynamicRectVsRect(const ColisionRect& moving, const ColisionRect& target, glm::vec2& contact_point, glm::vec2& contact_normal, float_t& contact_time, float_t deltaTime);

	};

}