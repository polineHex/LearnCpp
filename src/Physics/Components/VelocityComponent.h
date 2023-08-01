#pragma once

#include <raylib.h>

namespace game
{

struct VelocityComponent {
	Vector2 mDirection{};
	Vector2 mPrevDirection{};
	Vector2 mNewPossiblePosition{};
	float mSpeed{};

};
}// namespace game