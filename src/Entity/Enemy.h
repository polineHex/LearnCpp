#pragma once

#include <flecs.h>

namespace game
{

struct EnemyTag {};

namespace enemy
{

//TODO: get all this in a file and read from it
const int ENEMY_WIDTH = 16;
const int ENEMY_HEIGHT = 16;
const float ENEMY_SPEED = 150.f;

void InitEnemy(flecs::world& ecs);

}// namespace enemy	
}// namespace game
