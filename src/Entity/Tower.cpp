#include "Tower.h"

#include <iostream>

#include "Globals.h"

#include "Map/Map.h"

#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/HealthComponent.h"

#include "Physics/PhysicsUtils.h"
#include "Physics/Components/CollisionComponent.h"

#include "Character.h"
#include "Entity/Components/CoinsComponent.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/RenderUtils.h"

namespace game
{

// Forward declaration of `PlaceNewTower`.
// Only need to declare `static` once.
static void PlaceNewTower(flecs::iter& iter);

//TODO remove global prefab like with the enemy
flecs::entity gTowerPrefab{};
const Vector2 gTowerCollisionSize{TOWER_WIDTH * ENTITY_SCALE, TOWER_HEIGHT* ENTITY_SCALE};

void Tower::InitTower(flecs::world& ecs)
{
	gTowerPrefab = ecs.prefab<>("towerPrefab")
			.add<TowerTag>()
			.set_override<HealthComponent>({TOWER_MAX_HEALTH, TOWER_MAX_HEALTH})
			.override<TransformComponent>() // Need `override` because the data is not gonna be shared.
			.emplace<CollisionComponent>(gTowerCollisionSize) 
			.override<SpriteComponent>();

	ecs.system("PlaceNewTower")
			.kind(flecs::OnUpdate)
			.iter(PlaceNewTower);

}

void PlaceNewTower(flecs::iter& iter)
{
	if (!IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
		return;

	flecs::world ecs = iter.world();
	const flecs::entity& playerEntity = ecs.component<CharacterTag>().target<CharacterTag>();
	auto coinsComponent = playerEntity.get_ref<CoinsComponent>();
	const int coinsAmount = coinsComponent->mCoinAmount;

	if (coinsAmount < TOWER_PRICE)
		return;


	//Checking map bounds
	const flecs::entity& mapEntity = ecs.component<MapTag>().target<MapTag>();
	const Vector2 mousePosition = GetMousePosition();
	if (!map::IsWithinMapBounds(mapEntity, mousePosition, gTowerCollisionSize))
		return;

	const TransformComponent transformComponent{{mousePosition.x, mousePosition.y}, gTowerCollisionSize};
	const Rectangle newTowerRect{transformComponent.mPosition.x, transformComponent.mPosition.y, gTowerCollisionSize.x, gTowerCollisionSize.y};

	bool hasCollided = physicsUtils::RectCollision(ecs, newTowerRect);

	if (hasCollided)
		return;

	const int spriteIndex = GetRandomValue(0, 3);

	static int index = 0;
	const std::string towerName = std::string("tower" + std::to_string(index++));
	ecs.entity(towerName.c_str())
			.is_a(gTowerPrefab)
			.set<TransformComponent>(transformComponent)
			.set<SpriteComponent>({renderUtils::LoadMyTexture("buildings/towers_spritesheet_16x32_4x1.png"),
								   {(float)spriteIndex * TOWER_WIDTH, 0, TOWER_WIDTH, TOWER_HEIGHT},
								   {transformComponent.mScale.x,transformComponent.mScale.x}, {0, 0}, 1});

	//Paying the price
	coinsComponent->mCoinAmount -= TOWER_PRICE;
}


}// namespace game

