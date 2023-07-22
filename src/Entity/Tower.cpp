#include "Tower.h"

#include "Rendering/RenderUtils.h"

#include "Entity/Components/TransformComponent.h"
#include "Physics/Components/CollisionComponent.h"
#include "Rendering/Components/SpriteComponent.h"

namespace game
{

// Forward declaration of `PlaceNewTower`.
// Only need to declare `static` once.
static void PlaceNewTower(flecs::iter& iter);

flecs::entity gTowerPrefab{};

void Tower::InitTower(flecs::world& ecs)
{
	// Register components QUESTION:why we didnt have to do it before?
	ecs.component<CollisionComponent>();

	ecs.system<>("PlaceNewTower")
			.kind(flecs::OnUpdate)
			.iter(PlaceNewTower);

	gTowerPrefab = ecs.prefab<>("towerPrefab")
			.add<TowerTag>()
			.override<TransformComponent>() // Need `override` because the data is not gonna be shared.
			.override<CollisionComponent>() // This can be changed to `set` once `CollisionComponent` has been changed to only have the size of the sprite.
			.override<SpriteComponent>();

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
	const int spriteIndex = GetRandomValue(0, 3);

	//it doesnt account for the scale, hardcoding it really starts to bite me in the ass - redo asap
	const Rectangle newTowerRect{mousePosition.x, mousePosition.y, TOWER_WIDTH * 2, TOWER_HEIGHT * 2};

	bool hasCollided{false};
	iter.world().each<CollisionComponent>([&hasCollided, &newTowerRect](CollisionComponent& collisionComponent) {
		if (CheckCollisionRecs(newTowerRect, collisionComponent.mCollisionRect))
			hasCollided = true;
	});

	if (hasCollided)
		return;

	//If no collision was detected, we create a tower
	//TODO:redo collision component to only have dimension and use transform as a starting point of a rect
	//it doesnt account for the scale, hardcoding it really starts to bite me in the ass - redo asap!!

	iter.world().entity()
			.is_a(gTowerPrefab) // With `is_a`, we're making an instance of gTowerPrefab, and so we get all its components.
			.set<TransformComponent>({mousePosition.x, mousePosition.y})
			.set<CollisionComponent>({mousePosition.x, mousePosition.y, TOWER_WIDTH * 2, TOWER_HEIGHT * 2})
			.set<SpriteComponent>({RenderUtils::LoadMyTexture("buildings/towers_spritesheet_16x32_4x1.png"),
								   {(float)spriteIndex * TOWER_WIDTH, 0, TOWER_WIDTH, TOWER_HEIGHT},
								   {TOWER_WIDTH * 2, TOWER_HEIGHT * 2}, {0, 0}, 0});
}


}// namespace game

