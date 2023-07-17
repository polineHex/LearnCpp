#pragma once

#include <raylib.h>

namespace game
{

struct TransformComponent
{
	Vector2 mPosition{};
	Vector2 mScale{1.0f, 1.0f};
};

}// namespace game