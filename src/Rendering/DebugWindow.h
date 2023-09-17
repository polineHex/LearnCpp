
#pragma once

#include "flecs.h"

namespace game::Rendering
{

class DebugWindow
{
public:
	static void Draw(flecs::world& ecs);
};

} // namespace game::Rendering