#pragma once

#include <flecs.h>

#include "Entity/Components/TransformComponent.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/Components/AnimationComponent.h"

namespace game
{

const int TOWER_WIDTH = 16;
const int TOWER_HEIGHT = 16;

struct TowerTag {
};

class Tower
{
public:
	static void InitTower(flecs::world& ecs);
};

}// namespace game


//TODO move to the folder Buildings
