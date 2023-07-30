#pragma once

#include <flecs.h>

#include "Entity/Components/TransformComponent.h"
#include "Physics/Components/CollisionComponent.h"
#include "Physics/Components/VelocityComponent.h"
#include "Rendering/Components/AnimationStateComponent.h"
#include "Rendering/Components/SpriteComponent.h"

namespace game
{
// TODO: namespace entity
// void InitEnemy(...);

//TODO: get all this in a file and read from it
const int ENEMY_WIDTH = 16;
const int ENEMY_HEIGHT = 16;
const float ENEMY_SPEED = 150.f;

struct EnemyTag {
};


class Enemy
{
public:
	static void InitEnemy(flecs::world& ecs);

private:
	static void EnemyUpdate(flecs::entity enemyEntity, TransformComponent& transformComponent,
								AnimationStateComponent& animationStateComponent, 
								CollisionComponent& collisionComponent, VelocityComponent& velocityComponent);
};

}// namespace game
