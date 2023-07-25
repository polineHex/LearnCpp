#include "Character.h"

#include <raymath.h>

#include "Rendering/RenderUtils.h"
#include "Rendering/Components/TextureComponent.h"
#include "Rendering/Components/AnimationComponent.h"
#include "Map/Map.h"
#include "Globals.h"

namespace game
{

void Character::InitCharacter(flecs::world& ecs)
{
	//auto q = ecs.query<CollisionComponent, TransformComponent>();

	const TransformComponent transformComponent{Vector2{100, 100}, Vector2Scale(Vector2{CHARACTER_WIDTH, CHARACTER_HEIGHT}, ENTITY_SCALE)};
	ecs.entity("Player")
			.add<CharacterTag>()
			.emplace<TransformComponent>(transformComponent)
			.set<CollisionComponent>({transformComponent.mScale})
			.set<SpriteComponent>({renderUtils::LoadMyTexture("characters/knight_spritesheet_16x16_8x2.png"), 
									{0, 0, CHARACTER_WIDTH, CHARACTER_HEIGHT}, {transformComponent.mScale.x,transformComponent.mScale.x}, {0, 0}, 5})
			.emplace<AnimationComponent>(5, 0.1f, 0.0f, 0)
			.emplace<AnimationStateComponent>(AnimationName::IDLE);
	ecs.system<TransformComponent, AnimationStateComponent, CollisionComponent>()
			.with<CharacterTag>()
			.kind(flecs::OnUpdate)
			.each(CharacterUpdate);
}

void Character::CharacterUpdate(flecs::entity characterEntity, TransformComponent& transformComponent, 
								AnimationStateComponent& animationStateComponent, CollisionComponent& collisionComponent)
{
	Vector2 direction{};

	if (IsKeyDown(KEY_A))
		direction.x -= 1.0;
	else if (IsKeyDown(KEY_D))
		direction.x += 1.0;

	if (IsKeyDown(KEY_W))
		direction.y -= 1.0;
	else if (IsKeyDown(KEY_S))
		direction.y += 1.0;

	if (Vector2Length(direction) == 0.0)
	{
		animationStateComponent.mCurrentAnimName = AnimationName::IDLE;
		return;
	}
		
	//Changing animation row (aka switching to different animation) when running
	animationStateComponent.mCurrentAnimName = AnimationName::RUN;
	
	//Changing orientation of the character depending on where it's moving
	direction.x < 0.f ? transformComponent.mScale.x = -fabs(transformComponent.mScale.x) : transformComponent.mScale.x = fabs(transformComponent.mScale.x);
	
	//Check for map bounds
	const flecs::entity mapEntity = characterEntity.world().component<MapTag>().target<MapTag>();
	const Texture2D mapTexture = mapEntity.get_ref<TextureComponent>()->mTexture;

	const Vector2 newPosition = Vector2Add(transformComponent.mPosition, Vector2Scale(Vector2Normalize(direction), CHARACTER_SPEED * GetFrameTime()));

	// Check if the new position is within map boundaries 
	//TODO: Write a function to do it in map namespace? if i find myself using same code for towers/enemies
	if (newPosition.x >= 0 && newPosition.x <= mapTexture.width-CHARACTER_WIDTH &&
		newPosition.y >= 0 && newPosition.y <= mapTexture.height-CHARACTER_HEIGHT)
	{
		const Rectangle newCharacterRect {newPosition.x, newPosition.y, collisionComponent.mRectScale.x, collisionComponent.mRectScale.y};

		bool hasCollided{false};
		//auto q = ecs.query<CollisionComponent, TransformComponent>();
		//q.each([&hasCollided,&newCharacterRect, &characterEntity](flecs::entity e, CollisionComponent& otherCollisionComponent, TransformComponent& otherTransformComponent){
		//	// Avoid computing a collision detection if we already detected a collision.
		//	if (hasCollided)
		//		return;
		//	
		//	const Rectangle otherRect {otherTransformComponent.mPosition.x, otherTransformComponent.mPosition.y, otherCollisionComponent.mRectScale.x, otherCollisionComponent.mRectScale.y};
		//	if (e != characterEntity && CheckCollisionRecs(newCharacterRect, otherRect))
		//	{
		//		hasCollided = true;
		//	}
		//});

		//QUESTION: I couldnt pass two components into .each query. I tried to create query and use it, it leads to a crash. Apparently I cant create a query inside a system? 
		//passing a query from outside seems really complicated? Is there a better way to do each with entities that should have several components?
		//atm I just use transform scale, since it is the same (for rectangle collision) as collision component would be. 

		characterEntity.world().each<TransformComponent>([&hasCollided, &newCharacterRect, &characterEntity](flecs::entity e, TransformComponent& otherTransformComponent) {
			if (hasCollided)
				return;
			const Rectangle otherRect{otherTransformComponent.mPosition.x, otherTransformComponent.mPosition.y, otherTransformComponent.mScale.x, otherTransformComponent.mScale.y};
			if (e != characterEntity && CheckCollisionRecs(newCharacterRect, otherRect))
			{
				hasCollided = true;
			}
		});


		if (!hasCollided)
		{
			transformComponent.mPosition = newPosition;
		}
	}
}

}// namespace game