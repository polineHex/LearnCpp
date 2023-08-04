#include "Enemy.h"

#include <raylib.h>
#include <raymath.h>

#include "Globals.h"
#include "Map/Map.h"

#include "Entity/Character.h"
#include "Entity/Tower.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/TargetPlayerComponent.h"
#include "Entity/Components/TargetTowerComponent.h"

#include "Rendering/RenderUtils.h"

#include "Rendering/Components/AnimationComponent.h"
#include "Rendering/Components/AnimationStateComponent.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/Components/TextureComponent.h"

#include "Physics/PhysicsUtils.h"
#include "Physics/Components/CollisionComponent.h"
#include "Physics/Components/VelocityComponent.h"



namespace game
{
namespace enemy
{

flecs::entity gGoblinPrefab{};
static Vector2 gCharacterSize{ENEMY_WIDTH * ENTITY_SCALE, ENEMY_HEIGHT * ENTITY_SCALE};

static void SpawnNewEnemy(flecs::iter& iter);
static void EnemyUpdate(flecs::entity enemyEntity, TransformComponent& transformComponent, VelocityComponent& velocityComponent, const TargetTowerComponent& targetTowerComponentconst);

static TargetTowerComponent PickTowerTarget(flecs::world& ecs, const TransformComponent& transformComponent);


void InitEnemy(flecs::world& ecs)
{
	flecs::entity_t playerEntityRef = ecs.component<CharacterTag>().target<CharacterTag>().id();
	gGoblinPrefab = ecs.prefab<>("goblinPrefab")
							.add<EnemyTag>()
							.set<TargetPlayerComponent>({playerEntityRef})
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

	
	ecs.system<TransformComponent, VelocityComponent, TargetTowerComponent>()
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
	
	flecs::world ecs = iter.world();
	
	const TransformComponent transformComponent{{0.0f, 0.0f}, gCharacterSize};
	const Rectangle newGoblinRect{transformComponent.mPosition.x, transformComponent.mPosition.y, gCharacterSize.x, gCharacterSize.y};
	
	//Picking a tower, not spawning an enemy if there are no towers
	TargetTowerComponent targetTowerComponent = PickTowerTarget(ecs, transformComponent);
	if (targetTowerComponent.mTowerTarget == NULL)
		return;

	bool hasCollided = physicsUtils::RectCollision(ecs, newGoblinRect);
	if (hasCollided)
		return;
	
	
	
	//Setting the name of the goblin
	static int index = 0;
	const std::string goblinName = std::string("goblin" + std::to_string(index++));
	
	//Reset time only when spawn was succesfull
	lastSpawnTime = currentTime;
	
	ecs.entity(goblinName.c_str())
			.is_a(gGoblinPrefab)
			.set<TargetTowerComponent>(targetTowerComponent)
			.set<TransformComponent>(transformComponent);

}

TargetTowerComponent PickTowerTarget(flecs::world& ecs, const TransformComponent& transformComponent)
{
	
	//Getting random tower to go to (TODO: get the closest tower - function)
	auto filter = ecs.filter<TowerTag>();
	flecs::entity_t towerEntityRef{};

	filter.iter([&towerEntityRef](flecs::iter& it) {
		const int towerCount = it.count();
		if (towerCount == 0)
			return;

		const int towerIndex = GetRandomValue(0, towerCount-1);
		towerEntityRef = it.entity(towerIndex).id();
	});
	
	return {towerEntityRef};
}

void EnemyUpdate(flecs::entity enemyEntity, TransformComponent& transformComponent, VelocityComponent& velocityComponent, const TargetTowerComponent& targetTowerComponent)
{
	//Every tick we calculating next possible position for the enemy
	flecs::world ecs = enemyEntity.world();

	//TODO: update idling enemies with a tower then they have none and a new appears
	//TODO: decide where to run with enemies, when there are no towers

	if (targetTowerComponent.mTowerTarget == NULL)
		return;

	//auto playerEntity = ecs.entity(targetPlayerComponent.mPlayerTarget);
	//const Vector2 playerScreenPosition = playerEntity.get_ref<TransformComponent>()->mPosition;
	
	auto towerEntity = ecs.entity(targetTowerComponent.mTowerTarget);
	const Vector2 towerPosition = towerEntity.get_ref<TransformComponent>()->mPosition;
	
	
	velocityComponent.mPrevDirection = velocityComponent.mDirection;
	velocityComponent.mDirection = Vector2Subtract(towerPosition, transformComponent.mPosition);

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