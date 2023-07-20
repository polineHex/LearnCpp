#pragma once

#include <flecs.h>

#include "Rendering/RenderUtils.h"
#include "Entity/Components/TransformComponent.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/Components/AnimationStateComponent.h"

namespace game
{

const int CHARACTER_WIDTH = 16;
const int CHARACTER_HEIGHT = 16;
const float CHARACTER_SPEED = 4.f;


struct CharacterTag {
};

class Character
{
public:
	static void InitCharacter(flecs::world& ecs);
private:
	static void CharacterUpdate(flecs::entity characterEntity, TransformComponent& transformComponent, AnimationStateComponent& animationStateComponent);
};

}// namespace game
