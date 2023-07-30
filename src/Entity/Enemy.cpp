#include "Enemy.h"

#include <raymath.h>

#include "Globals.h"
#include "Map/Map.h"
#include "Entity/Character.h"
#include "Rendering/Components/AnimationComponent.h"
#include "Rendering/Components/TextureComponent.h"
#include "Rendering/RenderUtils.h"

namespace game
{

void Enemy::InitEnemy(flecs::world& ecs)
{
	const TransformComponent transformComponent{Vector2{500, 500}, Vector2Scale(Vector2{ENEMY_WIDTH, ENEMY_HEIGHT}, ENTITY_SCALE)};
	ecs.entity("Goblin")
			.add<EnemyTag>()
			.emplace<TransformComponent>(transformComponent)
			.set<CollisionComponent>({transformComponent.mScale})
			.set<VelocityComponent>({{0, 0}, ENEMY_SPEED})
			.set<SpriteComponent>({renderUtils::LoadMyTexture("characters/goblin_spritesheet_16x16_8x2.png"),
								   {0, 0, ENEMY_WIDTH, ENEMY_HEIGHT},
								   {transformComponent.mScale.x, transformComponent.mScale.x},
								   {0, 0},
								   5})
			.emplace<AnimationComponent>(5, 0.1f, 0.0f, 0)
			.emplace<AnimationStateComponent>(AnimationName::IDLE);
	
	ecs.system<TransformComponent, AnimationStateComponent, CollisionComponent, VelocityComponent>()
			.with<EnemyTag>()
			.kind(flecs::OnUpdate)
			.each(EnemyUpdate);
	
}

void Enemy::EnemyUpdate(flecs::entity enemyEntity, TransformComponent& transformComponent, AnimationStateComponent& animationStateComponent, 
								CollisionComponent& collisionComponent, VelocityComponent& velocityComponent)
{
	Vector2 playerScreenPosition, enemyScreenPosition;
	//QUESTION: how would i save the player ref on the init stage? Would it be another component as well? 
	playerScreenPosition = enemyEntity.world().component<CharacterTag>().target<CharacterTag>().get_ref<TransformComponent>()->mPosition;
	enemyScreenPosition = transformComponent.mPosition;
	Vector2 oldDirection = velocityComponent.mDirection;
	
	velocityComponent.mDirection = Vector2Subtract(playerScreenPosition, enemyScreenPosition);

	//TODO: get collision working here properly and get dimention from the collision object instead of 50
	if (Vector2Length(velocityComponent.mDirection) < 50.0f)
	{
		animationStateComponent.mCurrentAnimName = AnimationName::IDLE;
		velocityComponent.mDirection = {};
		return;
	}

	//Changing animation row (aka switching to different animation) when running
	animationStateComponent.mCurrentAnimName = AnimationName::RUN;

	//Changing orientation of the enemy depending on where it's moving
	bool oldDirectionSign = oldDirection.x < 0.f;
	bool newDirectionSign = velocityComponent.mDirection.x < 0.f;
	if (oldDirectionSign != newDirectionSign)
	{
		transformComponent.mScale.x = newDirectionSign ? -fabs(transformComponent.mScale.x) : fabs(transformComponent.mScale.x);
	}
	//Check for map bounds
	const flecs::entity mapEntity = enemyEntity.world().component<MapTag>().target<MapTag>();
	const Texture2D mapTexture = mapEntity.get_ref<TextureComponent>()->mTexture;

	const Vector2 newPosition = Vector2Add(transformComponent.mPosition, 
											Vector2Scale(Vector2Normalize(velocityComponent.mDirection), velocityComponent.mSpeed * GetFrameTime()));

	// Check if the new position is within map boundaries
	if (newPosition.x >= 0 && newPosition.x <= mapTexture.width - ENEMY_WIDTH &&
		newPosition.y >= 0 && newPosition.y <= mapTexture.height - ENEMY_HEIGHT)
	{
		const Rectangle newEntityRect{newPosition.x, newPosition.y, collisionComponent.mRectScale.x, collisionComponent.mRectScale.y};

		bool hasCollided{false};
		Rectangle otherRect{};
		auto filter = enemyEntity.world().filter<CollisionComponent, TransformComponent>();

		filter.each([&hasCollided, &newEntityRect, &otherRect, &enemyEntity](flecs::entity e, CollisionComponent& otherCollisionComponent, TransformComponent& otherTransformComponent) {
			// Avoid computing a collision detection if we already detected a collision.
			if (hasCollided)
				return;

			otherRect = {otherTransformComponent.mPosition.x, otherTransformComponent.mPosition.y, otherCollisionComponent.mRectScale.x, otherCollisionComponent.mRectScale.y};
			if (e != enemyEntity && CheckCollisionRecs(newEntityRect, otherRect))
			{
				hasCollided = true;
			}
		});


		if (!hasCollided)
		{
			transformComponent.mPosition = newPosition;
		}
	}
}

}// namespace game