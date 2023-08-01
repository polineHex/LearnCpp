#pragma once

#include <flecs.h>

#include <raylib.h>
#include <raymath.h>

namespace game
{

struct CharacterTag {};

namespace character
{

const int CHARACTER_WIDTH = 16;
const int CHARACTER_HEIGHT = 16;
const Vector2 CHARACTER_SPAWN_POINT = {100, 100};
const float CHARACTER_SPEED = 250.f;

void InitCharacter(flecs::world& ecs);

}//namespace character
}// namespace game
