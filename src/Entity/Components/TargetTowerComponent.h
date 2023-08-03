#pragma once

#include <flecs.h>

namespace game
{

//I split this in two components, cause player target is always same and can be inherited, and tower will be changed all the time
//But on the other hand, it's just a pointer, no harm in dragging it around? But at this point
//I follow: make the minimal possible division

struct TargetTowerComponent {
	//flecs::entity_t mPlayerTarget;
	flecs::entity_t mTowerTarget;
};

}// namespace game