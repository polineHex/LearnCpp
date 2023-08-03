#pragma once

#include <flecs.h>
#include <raylib.h>
#include <raymath.h>



namespace game
{

namespace physicsUtils
{


// Checks if the entity will collide with anything else, if placed in the newEntityRect
bool RectCollision(flecs::entity& entity, const Rectangle newEntityRect);

//Check if newly created entity will not collide with anything if put in the newEntityrect. There is 
//no entity yet, so we passing the world
bool RectCollision(flecs:: world& ecs, const Rectangle newEntityRect);


}// namespace physicsUtils
}// namespace game