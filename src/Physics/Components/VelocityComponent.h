#pragma once

#include <raylib.h>

namespace game
{

struct VelocityComponent {
	Vector2 mDirection{};
	Vector2 mPrevDirection{};
	
	//QUESTION: not happy that this is in velocity, but new position is possible only if there is velocity, so i dont want to drag it around
	//in transformComponent, but should i create an extra component just to store this?
	Vector2 mNewPossiblePosition{};
	float mSpeed{};

};
}// namespace game