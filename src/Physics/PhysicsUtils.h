#pragma once

#include <flecs.h>
#include <raylib.h>
#include <raymath.h>



namespace game
{

namespace physicsUtils
{


// Checks if the entity will collide with anything else, if placed in the newEntityRect.
// Default entity is passed as invalid entity in case we check collisions before creating an entity (for tower placing

bool RectCollision(flecs::world& ecs, const Rectangle newEntityRect, flecs::entity entity = flecs::entity());


}// namespace physicsUtils
}// namespace game