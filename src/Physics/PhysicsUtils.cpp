#include "PhysicsUtils.h"


#include "Entity/Components/TransformComponent.h"
#include "Physics/Components/CollisionComponent.h"

namespace game
{

namespace physicsUtils
{

bool RectCollision(flecs::world& ecs, const Rectangle newEntityRect, flecs::entity entity)
{
	bool hasCollided{false};
	auto filter = ecs.filter<CollisionComponent, TransformComponent>();

	filter.iter([&hasCollided, &newEntityRect, &entity](flecs::iter& it, CollisionComponent* otherCollisionComponents, TransformComponent* otherTransformComponents) {
		if (hasCollided)
			return;

		for (int i = 0; i < it.count(); ++i)
		{
			//In case we have passed a valid entity, we check if we are comparing entity with itself		
			if (it.entity(i) == entity)
				continue;
			
			auto otherCollisionComponent = otherCollisionComponents[i];
			auto otherTransformComponent = otherTransformComponents[i];
			
			Rectangle otherRect{otherTransformComponent.mPosition.x, otherTransformComponent.mPosition.y, otherCollisionComponent.mRectScale.x, otherCollisionComponent.mRectScale.y};
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