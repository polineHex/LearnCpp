#include "Enemy.h"

#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "Globals.h"
#include "Map/Map.h"

#include "Entity/Character.h"
#include "Entity/Tower.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/TargetTowerComponent.h"

#include "Rendering/RenderUtils.h"

#include "Rendering/Components/AnimationComponent.h"
#include "Rendering/Components/AnimationStateComponent.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/Components/TextureComponent.h"

#include "Physics/PhysicsUtils.h"
#include "Physics/Components/CollisionComponent.h"
#include "Physics/Components/VelocityComponent.h"

// Definitions of the global variables
int gWaveInProgressDebug = 0;
float gCurrentWaveDurationDebug = 0.0f;
float gCurrenSpawnDurationDebug = 0.0f;


namespace game
{
namespace enemy
{

flecs::entity gGoblinPrefab{};
static Vector2 gCharacterSize{ENEMY_WIDTH * ENTITY_SCALE, ENEMY_HEIGHT * ENTITY_SCALE};

//Forward declarations

static void SpawnNewEnemy(flecs::iter& iter);
static bool CheckAndStartNewWave(float& lastWaveEndTime, int& cornerIndex, float currentTime);
static bool IsWaveInProgress(const float lastWaveEndTime, const float currentTime);
static void SpawnEnemy(flecs::world& ecs, int cornerIndex, float& lastSpawnTime, float currentTime);
static flecs::entity_t GetClosestTowerId(flecs::world& ecs, const TransformComponent& enemyTransform);
static flecs::entity_t GetRandomTowerId(flecs::world& ecs);

static void EnemyUpdate(flecs::entity enemyEntity, TransformComponent& transformComponent, VelocityComponent& velocityComponent, const TargetTowerComponent& targetTowerComponentconst);

//Definitions

void InitEnemy(flecs::world& ecs)
{
	//flecs::entity_t playerEntityRef = ecs.component<CharacterTag>().target<CharacterTag>().id();
	gGoblinPrefab = ecs.prefab<>("goblinPrefab")
							.add<EnemyTag>()
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

//Spawn support functions
void SpawnNewEnemy(flecs::iter& iter)
{
	// Static variables to keep track of the state
	static float lastWaveEndTime = 0.0f;// Initialized to 0 to indicate that we haven't had a wave yet
	static float lastSpawnTime = 0.0f;
	static int cornerIndex = -1;// Will be set in the first wave

	float currentTime = GetTime();
	
	// Check if it's time to start a new wave and spawn an enemy
	if (!CheckAndStartNewWave(lastWaveEndTime, cornerIndex, currentTime))
		return;
	
	//DEBUG
	gCurrenSpawnDurationDebug = currentTime - lastSpawnTime;
	// Spawn a new enemy every ENEMIES_SPAWN_INTERVAL
	if (currentTime - lastSpawnTime < ENEMIES_SPAWN_INTERVAL)
		return;
	//Reset time only when spawn was succesfull?
	lastSpawnTime = currentTime;
	// Spawn the enemy
	SpawnEnemy(iter.world(), cornerIndex, lastSpawnTime, currentTime);

}

bool CheckAndStartNewWave(float& lastWaveEndTime, int& cornerIndex, float currentTime)
{
	// Determine the current state: wave or pause
	bool inWave = IsWaveInProgress(lastWaveEndTime, currentTime);

	if (!inWave)
	{
		// We're in a pause
		if (currentTime - lastWaveEndTime > PAUSE_DURATION)
		{
			// The pause is over, start a new wave
			lastWaveEndTime = currentTime + WAVE_DURATION;
			cornerIndex = GetRandomValue(0, 3);
			inWave = true;
		}
	}

	//DEBUG:
	gWaveInProgressDebug = inWave? 1 : 0;
	gCurrentWaveDurationDebug = currentTime - lastWaveEndTime;

	
	return inWave;
}


bool IsWaveInProgress(const float lastWaveEndTime, const float currentTime)
{
	// Checks if the wave is over
	if ((currentTime - lastWaveEndTime) >= (WAVE_DURATION + PAUSE_DURATION))
		return false;
	// Checks if the pause between waves is in progress
	if ((currentTime - lastWaveEndTime) <= PAUSE_DURATION)
		return false;
	return true;
}

void SpawnEnemy(flecs::world& ecs, int cornerIndex, float& lastSpawnTime, float currentTime)
{
	Vector2 spawnPosition{};
	
	//Picking map bounds to spawn from the corners of the map
	const Texture2D mapTexture = ecs.component<MapTag>().target<MapTag>().get_ref<TextureComponent>()->mTexture;

	switch (cornerIndex)
	{
		case 0:
			spawnPosition = {0.0f + gCharacterSize.x, 0.0f + gCharacterSize.y};
			break;
		case 1:
			spawnPosition = {0.0f + gCharacterSize.x, (float)mapTexture.height - gCharacterSize.y};
			break;
		case 2:
			spawnPosition = {(float)mapTexture.width - gCharacterSize.x, 0.0f + gCharacterSize.y};
			break;
		default:
			spawnPosition = {(float)mapTexture.width - gCharacterSize.x, (float)mapTexture.height - gCharacterSize.y};
			break;
	}
	
	const TransformComponent transformComponent{spawnPosition, gCharacterSize};
	const Rectangle newGoblinRect{transformComponent.mPosition.x, transformComponent.mPosition.y, gCharacterSize.x, gCharacterSize.y};

	bool hasCollided = physicsUtils::RectCollision(ecs, newGoblinRect);
	if (hasCollided)
		return;

	//Setting the name of the goblin
	static int index = 0;
	const std::string goblinName = std::string("goblin" + std::to_string(index++));
		
	ecs.entity(goblinName.c_str())
			.is_a(gGoblinPrefab)
			.set<TargetTowerComponent>(*ecs.entity(GetClosestTowerId(ecs, transformComponent)).get<TargetTowerComponent>())
			.set<TransformComponent>(transformComponent);
}

flecs::entity_t GetClosestTowerId(flecs::world& ecs, const TransformComponent& enemyTransform)
{
	auto filter = ecs.filter_builder<TransformComponent>().with<TowerTag>().build();
	flecs::entity_t closestTowerEntity = 0;
	
	// Initialize to the maximum possible float value
	float minDistance = std::numeric_limits<float>::max();
   
	filter.iter([&](flecs::iter& it, TransformComponent* towerTransformComponents) {
		for (auto i : it)
		{
			// Calculate the distance from this tower to the enemy
			float dx = towerTransformComponents[i].mPosition.x - enemyTransform.mPosition.x;
			float dy = towerTransformComponents[i].mPosition.y - enemyTransform.mPosition.y;
			float distance = sqrt(dx * dx + dy * dy);

			// If this tower is closer than the current closest tower, update the closest tower
			if (distance < minDistance)
			{
				minDistance = distance;
				closestTowerEntity = it.entity(i).id();
			}
		}
	});

	return {closestTowerEntity};
}

flecs::entity_t GetRandomTowerId(flecs::world& ecs)
{

	//Getting random tower to go to (TODO: get the closest tower - function)
	auto filter = ecs.filter<TowerTag>();
	flecs::entity_t towerEntityRef{};

	filter.iter([&towerEntityRef](flecs::iter& it) {
		const int towerCount = it.count();
		if (towerCount == 0)
			return;

		const int towerIndex = GetRandomValue(0, towerCount - 1);
		towerEntityRef = it.entity(towerIndex).id();
	});

	return {towerEntityRef};
}

//Movement and attack functions
void EnemyUpdate(flecs::entity enemyEntity, TransformComponent& transformComponent, VelocityComponent& velocityComponent, const TargetTowerComponent& targetTowerComponent)
{
	//Every tick we calculating next possible position for the enemy
	flecs::world ecs = enemyEntity.world();

	//TODO: update idling enemies with a tower then they have none and a new appears
	//TODO: decide where to run with enemies, when there are no towers

	if (targetTowerComponent.mTowerTarget == NULL)
		return;

	//Moving to player
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