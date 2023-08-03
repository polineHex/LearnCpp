#include "PhysicsUtils.h"


#include "Entity/Components/TransformComponent.h"
#include "Physics/Components/CollisionComponent.h"

namespace game
{

namespace physicsUtils
{

bool RectCollision(flecs::entity& entity, const Rectangle newEntityRect)
{
	bool hasCollided{false};
	Rectangle otherRect{};
	auto filter = entity.world().filter<CollisionComponent, TransformComponent>();

	filter.iter([&hasCollided, &newEntityRect, &otherRect, &entity](flecs::iter& it, CollisionComponent* otherCollisionComponents, TransformComponent* otherTransformComponents) {
		// Avoid computing a collision detection if we already detected a collision.
		if (hasCollided)
			return;

		for (int i = 0; i < it.count(); ++i)
		{
			auto otherEntity = it.entity(i);
			if (otherEntity == entity)
				continue;
			auto otherCollisionComponent = otherCollisionComponents[i];
			auto otherTransformComponent = otherTransformComponents[i];

			otherRect = {otherTransformComponent.mPosition.x, otherTransformComponent.mPosition.y, otherCollisionComponent.mRectScale.x, otherCollisionComponent.mRectScale.y};
			if (CheckCollisionRecs(newEntityRect, otherRect))
			{
				hasCollided = true;
				return;
			}
		}
	});

	return hasCollided;
}

bool RectCollision(flecs::world& ecs, const Rectangle newEntityRect)
{
	bool hasCollided{false};
	Rectangle otherRect{};
	auto filter = ecs.filter<CollisionComponent, TransformComponent>();

	filter.iter([&hasCollided, &newEntityRect, &otherRect](flecs::iter& it, CollisionComponent* otherCollisionComponents, TransformComponent* otherTransformComponents) {
		if (hasCollided)
			return;

		for (int i = 0; i < it.count(); ++i)
		{
			auto otherCollisionComponent = otherCollisionComponents[i];
			auto otherTransformComponent = otherTransformComponents[i];
			
			otherRect = {otherTransformComponent.mPosition.x, otherTransformComponent.mPosition.y, otherCollisionComponent.mRectScale.x, otherCollisionComponent.mRectScale.y};
			if (CheckCollisionRecs(newEntityRect, otherRect))
			{
				hasCollided = true;
				return;
			}
		}
	});

	return hasCollided;
}

}// namespace physicsUtils
}// namespace game