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
//QUESTION: why cant it be static? (compiler is unhappy with Vector2)
const Vector2 gTowerCollisionSize{TOWER_WIDTH * ENTITY_SCALE, TOWER_HEIGHT* ENTITY_SCALE};

void Tower::InitTower(flecs::world& ecs)
{
	gTowerPrefab = ecs.prefab<>("towerPrefab")
			.add<TowerTag>()
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

	const Vector2 mousePosition = GetMousePosition();
	const TransformComponent transformComponent{{mousePosition.x, mousePosition.y}, gTowerCollisionSize};
	
	//QUESTION: I am using transform here, but should get the data from collision component. Considering it's always same, should I add global const parameters for it?
	//what's the best way to "reuse it", considering i will in future have a collider that's different than sprite rect (a sphere on a slime)


	const Rectangle newTowerRect{transformComponent.mPosition.x, transformComponent.mPosition.y, gTowerCollisionSize.x, gTowerCollisionSize.y};

	bool hasCollided{false};
	auto filter = iter.world().filter<CollisionComponent, TransformComponent>();
	
	filter.iter([&hasCollided, &newTowerRect](flecs::iter& it, CollisionComponent* collisionComponents, TransformComponent* transformComponents) 
	{
		if (hasCollided)
			return;
		for (int i = 0; i < it.count(); ++i)
		{
		auto collisionComponent = collisionComponents[i];
		auto transformComponent = transformComponents[i];

		//QUESTION: how do i check that i am not checking the collision "with itself", aka i am checking Object
		//with all other Objects, so, I am bound to get collision with "itself"?

		//QUESTION: I didnt want to create a variable every iteration in a for loop, afaik, creating it many times is worse than having non-const being passed?
		const Rectangle otherRect = {transformComponent.mPosition.x, transformComponent.mPosition.y, collisionComponent.mRectScale.x, collisionComponent.mRectScale.y};
		
		if (CheckCollisionRecs(newTowerRect, otherRect))
			hasCollided = true;
		}
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

