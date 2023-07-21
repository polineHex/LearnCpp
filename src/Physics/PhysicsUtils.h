#pragma once

#include <raylib.h>

namespace game
{

class PhysicsUtils
{
public:
	static bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2);
	//TODO: Check collision with any shapes
};

}// namespace game