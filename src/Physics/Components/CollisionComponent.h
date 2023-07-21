#pragma once

#include <raylib.h>

namespace game
{

struct CollisionComponent {
	Rectangle mCollisionRect;
	
	//TODO: Make different shapes
	//TODO: split it into Vector2 and get position and scale from transform component
};
}// namespace game