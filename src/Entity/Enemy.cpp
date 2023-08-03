#include "Enemy.h"

#include <raylib.h>
#include <raymath.h>

#include "Globals.h"
#include "Map/Map.h"

#include "Entity/Character.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/TargetPlayerComponent.h"
#include "Entity/Components/TargetTowerComponent.h"

#include "Rendering/RenderUtils.h"

#include "Rendering/Components/AnimationComponent.h"
#include "Rendering/Components/AnimationStateComponent.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/Components/TextureComponent.h"

#include "Physics/Components/CollisionComponent.h"
#include "Physics/Components/VelocityComponent.h"



namespace game
{
namespace enemy
{

flecs::entity gGoblinPrefab{};
static Vector2 gCharacterSize{ENEMY_WIDTH * ENTITY_SCALE, ENEMY_HEIGHT * ENTITY_SCALE};

static void SpawnNewEnemy(flecs::iter& iter);
static void EnemyUpdate(flecs::entity enemyEntity, TransformComponent& transformComponent, VelocityComponent& velocityComponent, const TargetPlayerComponent& targetPlayerComponent);


void InitEnemy(flecs::world& ecs)
{
	flecs::entity* pt = &ecs.component<CharacterTag>().target<CharacterTag>();
	gGoblinPrefab = ecs.prefab<>("goblinPrefab")
							.add<EnemyTag>()
							.set<TargetPlayerComponent>({pt})
							.set_override<TargetTowerComponent>({})
							.override<TransformComponent>()
							.emplace<CollisionComponent>(gCharacterSize)
							.set_override<VelocityComponent>({{0, 0}, {0, 0}, {0, 0}, ENEMY_SPEED})
							.set_override<SpriteComponent>({renderUtils::LoadMyTexture("characters/goblin_spritesheet_16x16_8x2.png"),
															{0, 0, ENEMY_WIDTH, ENEMY_HEIGHT},
															{gCharacterSize.x, gCharacterSize.y},
															{0, 0},
															5})
							.emplace_override<AnimationComponent>(5, 0.1f, 0.0f, 0)
							.emplace_override<AnimationStateComponent>(AnimationName::IDLE);
							

	ecs.system("SpawnNewEnemy")
			.kind(flecs::OnUpdate)
			.iter(SpawnNewEnemy);

	ecs.system<TransformComponent, VelocityComponent, TargetPlayerComponent>()
			.with<EnemyTag>()
			.kind(flecs::OnUpdate)
			.each(EnemyUpdate);
}

void SpawnNewEnemy(flecs::iter& iter)
{
	static float lastSpawnTime = 0.0f;
	float currentTime = GetTime();

	if (currentTime - lastSpawnTime < 1.0f)// Spawn a new enemy every 1 seconds
		return;

	lastSpawnTime = currentTime;

	const TransformComponent transformComponent{{0.0f, 0.0f}, gCharacterSize};
	const Rectangle newGoblinRect{transformComponent.mPosition.x, transformComponent.mPosition.y, gCharacterSize.x, gCharacterSize.y};

	bool hasCollided{false};

	//Save world for efficiency
	flecs::world ecs = iter.world();
	
	auto filter = ecs.filter<CollisionComponent, TransformComponent>();

	filter.iter([&hasCollided, &newGoblinRect](flecs::iter& it, CollisionComponent* collisionComponents, TransformComponent* transformComponents) {
		if (hasCollided)
			return;
		for (int i = 0; i < it.count(); ++i)
		{
			auto otherCollisionComponent = collisionComponents[i];
			auto otherTransformComponent = transformComponents[i];
			const Rectangle otherRect{otherTransformComponent.mPosition.x, otherTransformComponent.mPosition.y, otherCollisionComponent.mRectScale.x, otherCollisionComponent.mRectScale.y};

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

void EnemyUpdate(flecs::entity enemyEntity, TransformComponent& transformComponent, VelocityComponent& velocityComponent, const TargetPlayerComponent& targetPlayerComponent)
{
	//Every tick we calculating next possible position for the enemy
	Vector2 playerScreenPosition, enemyScreenPosition;
	
	//DEBUG
	flecs::entity* pt1 = targetPlayerComponent.mPlayerTarget;
	flecs::entity* pt2 = &enemyEntity.world().component<CharacterTag>().target<CharacterTag>();
	
	auto player = *pt1;
	
	playerScreenPosition = player.get_ref<TransformComponent>()->mPosition;
	
	//playerScreenPosition = enemyEntity.world().component<CharacterTag>().target<CharacterTag>().get_ref<TransformComponent>()->mPosition;
	enemyScreenPosition = transformComponent.mPosition;
	
	velocityComponent.mPrevDirection = velocityComponent.mDirection;
	velocityComponent.mDirection = Vector2Subtract(playerScreenPosition, enemyScreenPosition);

	//TODO: get collision working here properly and get dimention from the collision object instead of 50
	if (Vector2Length(velocityComponent.mDirection) < 50.0f)
	{
		velocityComponent.mDirection = {};
		return;
	}

	velocityComponent.mNewPossiblePosition = Vector2Add(transformComponent.mPosition,
										   Vector2Scale(Vector2Normalize(velocityComponent.mDirection), velocityComponent.mSpeed * GetFrameTime()));
}


}// namespace enemy	
}// namespace game