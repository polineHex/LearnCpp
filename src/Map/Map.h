#pragma once

#include <flecs.h>

namespace game
{

struct MapTag{};

namespace map
{

void CreateMap(flecs::world& ecs);

} // namespace map

} // namespace game