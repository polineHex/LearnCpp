#include "Tower.h"

#include <iostream>

#include "Entity/Components/TransformComponent.h"
#include "Physics/Components/CollisionComponent.h"
#include "Rendering/RenderUtils.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Globals.h"

namespace game
{

// Forward declaration of `PlaceNewTower`.
// Only need to declare `static` once.
static void PlaceNewTower(flecs::iter& iter);

flecs::entity gTowerPrefab{};

void Tower::InitTower(flecs::world& ecs)
{
	gTowerPrefab = ecs.prefab<>("towerPrefab")
			.add<TowerTag>()
			.override<TransformComponent>() // Need `override` because the data is not gonna be shared.
			.set<CollisionComponent>({Vector2{TOWER_WIDTH*ENTITY_SCALE, TOWER_HEIGHT*ENTITY_SCALE}}) 
			.override<SpriteComponent>();

	ecs.system("PlaceNewTower")
			.kind(flecs::OnUpdate)
			.iter(PlaceNewTower);

}

void PlaceNewTower(flecs::iter& iter)
{
	if (!IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
		return;

	const Vector2 mousePosition = GetMousePosition();
	const TransformComponent transformComponent{Vector2{mousePosition.x, mousePosition.y}, Vector2{TOWER_WIDTH * ENTITY_SCALE, TOWER_HEIGHT*ENTITY_SCALE}};
	const Rectangle newTowerRect{transformComponent.mPosition.x, transformComponent.mPosition.y, transformComponent.mScale.x, transformComponent.mScale.y};

	bool hasCollided{false};

	//Replaced collision component for now with transform component, same issue as Character.cpp checking on collision on update, cause cant pass two components in query
	//Assuming at this point, that collision rect is the same as transform rect

	iter.world().each<TransformComponent>([&hasCollided, &newTowerRect](TransformComponent& otherTransformComponent) {
		// Avoid computing a collision detection if we already detected a collision.
		if (hasCollided)
			return;

		const Rectangle otherRect = {otherTransformComponent.mPosition.x, otherTransformComponent.mPosition.y, otherTransformComponent.mScale.x, otherTransformComponent.mScale.y};
		if (CheckCollisionRecs(newTowerRect, otherRect))
			hasCollided = true;
	});

	if (hasCollided)
		return;

	const int spriteIndex = GetRandomValue(0, 3);

	static int index = 0;
	const std::string towerName = std::string("tower" + std::to_string(index++));
	iter.world().entity(towerName.c_str())
			.is_a(gTowerPrefab)
			.set<TransformComponent>(transformComponent)
			.set<SpriteComponent>({renderUtils::LoadMyTexture("buildings/towers_spritesheet_16x32_4x1.png"),
								   {(float)spriteIndex * TOWER_WIDTH, 0, TOWER_WIDTH, TOWER_HEIGHT},
								   {transformComponent.mScale.x,transformComponent.mScale.x}, {0, 0}, 0});
}


}// namespace game

