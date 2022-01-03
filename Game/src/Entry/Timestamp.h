#pragma once

namespace Game {

	struct Timestamp
	{
		float dt;

		Timestamp(double p_dt) :dt(p_dt) {}

		operator float() const { return dt; }
		operator float& () { return dt; }
	};
}