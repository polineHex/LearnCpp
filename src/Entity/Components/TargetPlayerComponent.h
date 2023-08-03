#pragma once

#include <flecs.h>

namespace game
{

struct TargetPlayerComponent {
	flecs::entity_t mPlayerTarget;
};

}// namespace game