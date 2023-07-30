#pragma once

#include <raylib.h>

namespace game
{

struct VelocityComponent {
	Vector2 mDirection{};
	float mSpeed{};

};
}// namespace game