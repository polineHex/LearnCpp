#pragma once

#include <flecs.h>

#include <raylib.h>
#include <raymath.h>

namespace game
{

struct MapTag{};

namespace map
{

void CreateMap(flecs::world& ecs);
bool IsWithinMapBounds(const flecs::entity& mapEntity, const Vector2 newPosition, const Vector2 collisionSize);

} // namespace map

} // namespace game