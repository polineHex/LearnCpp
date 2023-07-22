#pragma once

#include <flecs.h>

#include "Entity/Components/TransformComponent.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/Components/AnimationStateComponent.h"
#include "Physics/Components/CollisionComponent.h"

namespace game
{

const int CHARACTER_WIDTH = 16;
const int CHARACTER_HEIGHT = 16;
const float CHARACTER_SPEED = 250.f;


struct CharacterTag {
};

class Character
{
public:
	static void InitCharacter(flecs::world& ecs);
private:
	static void CharacterUpdate(flecs::entity characterEntity, TransformComponent& transformComponent, 
								AnimationStateComponent& animationStateComponent, CollisionComponent& collisionComponent);
};

}// namespace game
