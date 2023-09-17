#pragma once

#include <flecs.h>

namespace game
{

struct TargetTowerComponent {
	flecs::entity_t mTowerTarget;
};

}// namespace game