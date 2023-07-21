#include "Character.h"

#include <raymath.h>

#include "Rendering/RenderUtils.h"
#include "Physics/PhysicsUtils.h"
#include "Rendering/Components/TextureComponent.h"
#include "Rendering/Components/AnimationComponent.h"

namespace game
{

void Character::InitCharacter(flecs::world& ecs)
{
	//TODO: define components first, and then assign them, to link relevant data (texture width)
	ecs.entity("Player")
			.add<CharacterTag>()
			.set<TransformComponent>({100, 100})
			.set<CollisionComponent>({100, 100, CHARACTER_WIDTH * 2, CHARACTER_HEIGHT * 2})
			.set<SpriteComponent>({RenderUtils::LoadMyTexture("characters/knight_spritesheet_16x16_8x2.png"), 
									{0, 0, CHARACTER_WIDTH, CHARACTER_HEIGHT}, {CHARACTER_WIDTH * 2, CHARACTER_HEIGHT * 2}, {0, 0}, 5})
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
	int frame{};

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
	direction.x < 0.f ? transformComponent.mScale.x = -1.f : transformComponent.mScale.x = 1.f;
	
	//Check for map bounds
	flecs::entity mapEntity = characterEntity.world().entity("Map");
	Texture2D mapTexture = mapEntity.get_ref<TextureComponent>()->mTexture;

	// Calculate new position
	Vector2 newPosition = Vector2Add(transformComponent.mPosition, Vector2Scale(Vector2Normalize(direction), CHARACTER_SPEED));

	// Check if the new position is within map boundaries 
	//QUESTION: if i want this as function, would it be a system on map? how do i then call one?
	if (newPosition.x >= 0 && newPosition.x <= mapTexture.width-CHARACTER_WIDTH &&
		newPosition.y >= 0 && newPosition.y <= mapTexture.height-CHARACTER_HEIGHT)
	{
		  // Create a new rectangle that represents where the character would move
		Rectangle newCharacterRect = {newPosition.x, newPosition.y, collisionComponent.mCollisionRect.width, collisionComponent.mCollisionRect.height};

		bool isCollision = false;

		// Iterate over all entities that have a CollisionComponent
		characterEntity.world().each<CollisionComponent>([&](flecs::entity e, CollisionComponent& otherCollisionComponent) {
			if (e != characterEntity && PhysicsUtils::CheckCollisionRecs(newCharacterRect, otherCollisionComponent.mCollisionRect))
			{
				isCollision = true;
			}
		});

		if (!isCollision)
		{
			transformComponent.mPosition = newPosition;
		}
	}
}

}// namespace game