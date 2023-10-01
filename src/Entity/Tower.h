#pragma once

#include <flecs.h>


namespace game
{

const int TOWER_WIDTH = 16;
const int TOWER_HEIGHT = 16;
const int TOWER_MAX_HEALTH = 100;
const int TOWER_PRICE = 2;

struct TowerTag {
};

class Tower
{
public:
	static void InitTower(flecs::world& ecs);
};

}// namespace game


//TODO move to the folder Buildings
// TODO move to namespace
