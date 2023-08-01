#include "Entity.h"

#include "Map/Map.h"

#include "Entity/Character.h"
#include "Entity/Enemy.h"

#include "Entity/Components/TransformComponent.h"

#include "Physics/Components/CollisionComponent.h"
#include "Physics/Components/VelocityComponent.h"
#include "Rendering/Components/AnimationStateComponent.h"
#include "Rendering/Components/TextureComponent.h"


namespace game
{

namespace entity
{
	
void UpdateEntityAnimationState(flecs::entity characterEntity, AnimationStateComponent& animationStateComponent, const VelocityComponent& velocityComponent);
void UpdateEntityOrientationX(flecs::entity entity, TransformComponent& transformComponent, const VelocityComponent& velocityComponent);
void UpdateEntityPosition(flecs::entity entity, TransformComponent& transformComponent, const VelocityComponent& velocityComponent, const CollisionComponent& collisionComponent);


void InitEntity(flecs::world& ecs)
{
	ecs.system<AnimationStateComponent, const VelocityComponent>()
			.kind(flecs::OnUpdate)
			.each(UpdateEntityAnimationState);
	
	ecs.system<TransformComponent, const VelocityComponent>()
			.kind(flecs::OnUpdate)
			.each(UpdateEntityOrientationX);
	
	ecs.system<TransformComponent, const VelocityComponent, const CollisionComponent>()
			.kind(flecs::OnUpdate)
			.each(UpdateEntityPosition);
	
	
	//QUESTION: is it okay to do like this?
	character::InitCharacter(ecs);
	enemy::InitEnemy(ecs);

}

void UpdateEntityAnimationState(flecs::entity entity, AnimationStateComponent& animationStateComponent, const VelocityComponent& velocityComponent)
{
	if (Vector2Length(velocityComponent.mDirection) == 0.0f)
	{
		animationStateComponent.mCurrentAnimName = AnimationName::IDLE;
		return;
	}

	//Changing animation row (aka switching to different animation) when running
	animationStateComponent.mCurrentAnimName = AnimationName::RUN;
}

void UpdateEntityOrientationX(flecs::entity entity, TransformComponent& transformComponent, const VelocityComponent& velocityComponent)
{
	
	//If we are not moving we dont need to update
	if (Vector2Length(velocityComponent.mDirection) == 0.0f)
		return;

	//Flipping orientation of the character if direction sign flipped as well
	if (velocityComponent.mPrevDirection.x != velocityComponent.mDirection.x)
	{
		transformComponent.mScale.x = (velocityComponent.mDirection.x < 0.0f) ? -fabs(transformComponent.mScale.x) : fabs(transformComponent.mScale.x);
	}
}

void UpdateEntityPosition(flecs::entity entity, TransformComponent& transformComponent, const VelocityComponent& velocityComponent, const CollisionComponent& collisionComponent)
{
	//If we are not moving we dont need to update
	if (Vector2Length(velocityComponent.mDirection) == 0.0f)
		return;

	//Check for map bounds
	const flecs::entity& mapEntity = entity.world().component<MapTag>().target<MapTag>();

	if (!map::IsWithinMapBounds(mapEntity, velocityComponent.mNewPossiblePosition, collisionComponent.mRectScale))
		return;
	
	//Check if we are colliding with anything when moved
	const Rectangle newEntityRect{velocityComponent.mNewPossiblePosition.x, velocityComponent.mNewPossiblePosition.y, 
								collisionComponent.mRectScale.x, collisionComponent.mRectScale.y};
	Rectangle otherRect{};
	bool hasCollided{false};

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

			const Rectangle otherRect = {otherTransformComponent.mPosition.x, otherTransformComponent.mPosition.y, otherCollisionComponent.mRectScale.x, otherCollisionComponent.mRectScale.y};
			if (CheckCollisionRecs(newEntityRect, otherRect))
			{
				hasCollided = true;
				return;
			}
		}
	});

	if (!hasCollided)
	{
		transformComponent.mPosition = velocityComponent.mNewPossiblePosition;
	}
	
}


}//namesapce entity
}//namespace