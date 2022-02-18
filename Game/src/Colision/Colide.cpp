#include "pch.h"
#include "Colide.h"

namespace Game
{

	bool Colide::RectVsRect(const ColisionRect& t, const ColisionRect& f)
	{
		if (t.pos.x < f.pos.x + f.size.x &&
			t.pos.x + t.size.x > f.pos.x &&
			t.pos.y < f.pos.y + f.size.y &&
			t.pos.y + t.size.y > f.pos.y)
		{
			return true;
		}
		return false;
	}

	bool Colide::PointVsRect(const glm::vec2& p, const ColisionRect& r)
	{
		return (p.x >= r.pos.x && p.y >= r.pos.y && p.x < r.pos.x + r.size.x && p.y < r.pos.y + r.size.y);
	}

	bool Colide::RayVsRect(const glm::vec2& ray_origin, const glm::vec2& ray_dir, const ColisionRect& target, glm::vec2& contact_point, glm::vec2& contact_normal, float_t& t_hit_near)
	{
		glm::vec2 t_near = (target.pos - ray_origin) / ray_dir;
		glm::vec2 t_far = (target.pos + target.size - ray_origin) / ray_dir;

		if (std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
		if (std::isnan(t_near.y) || std::isnan(t_near.x)) return false;

		if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
		if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

		if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

		t_hit_near = std::max(t_near.x, t_near.y);
		float t_hit_far = std::min(t_far.x, t_far.y);

		if (t_hit_far < 0) return false;

		contact_point = ray_origin + t_hit_near * ray_dir;

		if (t_near.x > t_near.y)
			if (ray_dir.x < 0)
				contact_normal = { 1, 0 };
			else
				contact_normal = { -1, 0 };
		else if (t_near.x < t_near.y)
			if (ray_dir.y < 0)
				contact_normal = { 0, 1 };
			else
				contact_normal = { 0, -1 };

		return true;
	}

	bool Colide::DynamicRectVsRect(const ColisionRect& moving, const ColisionRect& target, glm::vec2& contact_point, glm::vec2& contact_normal, float_t& contact_time, float_t deltaTime)
	{
		if (moving.velocity.x == 0 && moving.velocity.y == 0)
			return false;

		ColisionRect expanded_target{};
		expanded_target.pos = target.pos - moving.size / 2.0f;
		expanded_target.size = target.size + moving.size;

		if (RayVsRect(moving.pos + moving.size / 2.0f, moving.velocity * deltaTime, expanded_target, contact_point, contact_normal, contact_time))
		{
			if (contact_time >= 0.0f && contact_time <= 1.0f)
				return true;
		}
		return false;
	}

}