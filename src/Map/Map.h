#pragma once

#include <flecs.h>

namespace game
{
class Map
{
public:
	static void CreateMap(flecs::world& ecs);
};

} // namespace game