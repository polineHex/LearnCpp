#pragma once

#include <flecs.h>

namespace game
{

struct TargetPlayerComponent {
	flecs::entity* mPlayerTarget;
};

}// namespace game