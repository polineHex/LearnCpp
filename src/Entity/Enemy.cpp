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
//QUESTION: would i declare all private static functions with forward declaratiobn?

static void SpawnNewEnemy(flecs::iter& iter);
flecs::entity gGoblinPrefab{};

void Enemy::InitEnemy(flecs::world& ecs)
{
	const TransformComponent transformComponent{Vector2{500, 500}, Vector2Scale(Vector2{ENEMY_WIDTH, ENEMY_HEIGHT}, ENTITY_SCALE)};
	
	gGoblinPrefab = ecs.prefab<>("goblinPrefab")
							.add<EnemyTag>()
							.override<TransformComponent>()
							.set<CollisionComponent>({ENEMY_WIDTH * ENTITY_SCALE, ENEMY_HEIGHT * ENTITY_SCALE})
							.set_override<VelocityComponent>({{0, 0}, ENEMY_SPEED})
							.set_override<SpriteComponent>({renderUtils::LoadMyTexture("characters/goblin_spritesheet_16x16_8x2.png"),
												   {0, 0, ENEMY_WIDTH, ENEMY_HEIGHT},
												   {transformComponent.mScale.x, transformComponent.mScale.x},
												   {0, 0},
												   5})
							.emplace_override<AnimationComponent>(5, 0.1f, 0.0f, 0)
							.emplace_override<AnimationStateComponent>(AnimationName::IDLE);

	ecs.system("SpawnNewEnemy")
			.kind(flecs::OnUpdate)
			.iter(SpawnNewEnemy);

	ecs.system<TransformComponent, AnimationStateComponent, CollisionComponent, VelocityComponent>()
			.with<EnemyTag>()
			.kind(flecs::OnUpdate)
			.each(EnemyUpdate);
	
}

void SpawnNewEnemy(flecs::iter& iter)
{
	static float lastSpawnTime = 0.0f;
	float currentTime = GetTime();

	if (currentTime - lastSpawnTime < 1.0f)// Spawn a new enemy every 5 seconds
		return;

	lastSpawnTime = currentTime;

	const TransformComponent transformComponent{{0.0f, 0.0f}, {ENEMY_WIDTH * ENTITY_SCALE, ENEMY_HEIGHT * ENTITY_SCALE}};
	const Rectangle newGoblinRect{transformComponent.mPosition.x, transformComponent.mPosition.y, transformComponent.mScale.x, transformComponent.mScale.y};

	bool hasCollided{false};
	
	//Save world for efficiency 
	flecs::world ecs = iter.world();
	auto filter = ecs.filter<CollisionComponent, TransformComponent>();

	filter.iter([&hasCollided, &newGoblinRect](flecs::iter& it, CollisionComponent* collisionComponents, TransformComponent* transformComponents) {
		if (hasCollided)
			return;
		for (int i = 0; i < it.count(); ++i)
		{
			auto collisionComponent = collisionComponents[i];
			auto transformComponent = transformComponents[i];
			const Rectangle otherRect{transformComponent.mPosition.x, transformComponent.mPosition.y, collisionComponent.mRectScale.x, collisionComponent.mRectScale.y};

			if (CheckCollisionRecs(newGoblinRect, otherRect))
			{
				hasCollided = true;
				return;
			}
		}
	});

	if (hasCollided)
		return;

	static int index = 0;
	const std::string goblinName = std::string("goblin" + std::to_string(index++));
	ecs.entity(goblinName.c_str())
			.is_a(gGoblinPrefab)
			.set<TransformComponent>(transformComponent);
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

	//Flipping orientation of the character if direction sign flipped as well
	if (oldDirection.x != velocityComponent.mDirection.x)
	{
		transformComponent.mScale.x = (velocityComponent.mDirection.x < 0.0f) ? -fabs(transformComponent.mScale.x) : fabs(transformComponent.mScale.x);
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

		filter.iter([&hasCollided, &newEntityRect, &otherRect, &enemyEntity](flecs::iter& it, CollisionComponent* collisionComponents, TransformComponent* transformComponents) {
			// Avoid computing a collision detection if we already detected a collision.
			if (hasCollided)
				return;

			for (int i = 0; i < it.count(); ++i)
			{
				auto entity = it.entity(i);
				if (entity == enemyEntity)
					continue;
				auto collisionComponent = collisionComponents[i];
				auto transformComponent = transformComponents[i];
				
				const Rectangle otherRect = {transformComponent.mPosition.x, transformComponent.mPosition.y, collisionComponent.mRectScale.x, collisionComponent.mRectScale.y};
				if (CheckCollisionRecs(newEntityRect, otherRect))
				{
					hasCollided = true;
				}
			}
			
		});

		if (!hasCollided)
		{
			transformComponent.mPosition = newPosition;
		}
	}
}

}// namespace game