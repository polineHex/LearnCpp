#include "Tower.h"

#include <iostream>

#include "Entity/Components/TransformComponent.h"
#include "Physics/Components/CollisionComponent.h"
#include "Rendering/RenderUtils.h"
#include "Rendering/Components/SpriteComponent.h"

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
			.override<CollisionComponent>() // This can be changed to `set` once `CollisionComponent` has been changed to only have the size of the sprite.
			.override<SpriteComponent>();

	ecs.system("PlaceNewTower")
			.kind(flecs::OnUpdate)
			.iter(PlaceNewTower);

	// Entity A + collisionComponent
	// Entity B + collisionComponent

//	ecs.system<CollisionComponent>()
//	        .iter([](flecs::iter iter, CollisionComponent* collisionComponent) {
//				// You get an array of all the CollisionComponent.
//				// CollisionComponent for entity A + B.
//				for (int i = 0; i < iter.count(); ++i)
//				{
//					CollisionComponent collisionComponent1 = collisionComponent[i];
//				}
//			});
//
//	ecs.system<CollisionComponent>()
//	        .each([](flecs::entity entity, CollisionComponent& collisionComponent) {
//				// You get the reference to CollisionComponent for the entity you're given.
//				// This means you only have one CollisionComponent.
//			});
}

void PlaceNewTower(flecs::iter& iter)
{
	if (!IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
		return;

	const Vector2 mousePosition = GetMousePosition();
	const Rectangle newTowerRect{mousePosition.x, mousePosition.y, TOWER_WIDTH * 2, TOWER_HEIGHT * 2};

	bool hasCollided{false};

	iter.world().each<CollisionComponent>([&hasCollided, &newTowerRect](CollisionComponent& collisionComponent) {
		// Avoid computing a collision detection if we already detected a collision.
		if (hasCollided)
			return;

		if (CheckCollisionRecs(newTowerRect, collisionComponent.mCollisionRect))
			hasCollided = true;
	});

	if (hasCollided)
		return;

	//If no collision was detected, we create a tower
	//TODO:redo collision component to only have dimension and use transform as a starting point of a rect
	//it doesnt account for the scale, hardcoding it really starts to bite me in the ass - redo asap!!

	const int spriteIndex = GetRandomValue(0, 3);

	static int index = 0;
	const std::string towerName = std::string("tower" + std::to_string(index++));
	iter.world().entity(towerName.c_str())
			.is_a(gTowerPrefab)
			.set<TransformComponent>({mousePosition.x, mousePosition.y})
			.set<CollisionComponent>({newTowerRect})
			.set<SpriteComponent>({RenderUtils::LoadMyTexture("buildings/towers_spritesheet_16x32_4x1.png"),
								   {(float)spriteIndex * TOWER_WIDTH, 0, TOWER_WIDTH, TOWER_HEIGHT},
								   {TOWER_WIDTH * 2, TOWER_HEIGHT * 2}, {0, 0}, 0});
}


}// namespace game

