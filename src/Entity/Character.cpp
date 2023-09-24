#include "Character.h"

#include <raylib.h>
#include <raymath.h>

#include "Globals.h"
#include "Map/Map.h"

#include "Entity/Components/TransformComponent.h"

#include "Rendering/RenderUtils.h"

#include "Rendering/Components/AnimationComponent.h"
#include "Rendering/Components/AnimationStateComponent.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/Components/TextureComponent.h"

#include "Physics/Components/CollisionComponent.h"
#include "Physics/Components/VelocityComponent.h"

namespace game
{

namespace character
{
static Vector2 gCharacterSize{CHARACTER_WIDTH * ENTITY_SCALE, CHARACTER_HEIGHT * ENTITY_SCALE};

static void CharacterInput(flecs::entity characterEntity, VelocityComponent& velocityComponent);
static void CharacterUpdate(flecs::entity characterEntity, TransformComponent& transformComponent, VelocityComponent& velocityComponent);

void InitCharacter(flecs::world& ecs)
{
	const TransformComponent transformComponent{CHARACTER_SPAWN_POINT, gCharacterSize};

	flecs::entity playerEntity = ecs.entity("Player")
									.add<CharacterTag>()
									.emplace<TransformComponent>(transformComponent)
									.emplace<CollisionComponent>(gCharacterSize)
									.set<VelocityComponent>({{0, 0}, {0, 0}, transformComponent.mPosition, CHARACTER_SPEED})
									.set<SpriteComponent>({renderUtils::LoadMyTexture("characters/knight_spritesheet_16x16_8x2.png"),
														{0, 0, CHARACTER_WIDTH, CHARACTER_HEIGHT},
														{transformComponent.mScale.x, transformComponent.mScale.y},
														{0, 0},
														1})
									.emplace<AnimationComponent>(5, 0.1f, 0.0f, 0)
									.emplace<AnimationStateComponent>(AnimationName::IDLE);

	ecs.add<CharacterTag>(playerEntity);

	//Order is important. Input is before update
	ecs.system<VelocityComponent>()
			.with<CharacterTag>()
			.kind(flecs::PreUpdate)
			.each(CharacterInput);

	//Logic on update
	ecs.system<TransformComponent, VelocityComponent>()
			.with<CharacterTag>()
			.kind(flecs::OnUpdate)
			.each(CharacterUpdate);

	//Rendering is after logic is done e.g. PreStore/PostUpdate
}

void CharacterInput(flecs::entity characterEntity, VelocityComponent& velocityComponent)
{
	velocityComponent.mPrevDirection = velocityComponent.mDirection;
	velocityComponent.mDirection = {0.0f, 0.0f};

	if (IsKeyDown(KEY_A))
		velocityComponent.mDirection.x -= 1.0f;
	else if (IsKeyDown(KEY_D))
		velocityComponent.mDirection.x += 1.0f;

	if (IsKeyDown(KEY_W))
		velocityComponent.mDirection.y -= 1.0f;
	else if (IsKeyDown(KEY_S))
		velocityComponent.mDirection.y += 1.0f;
}

void CharacterUpdate(flecs::entity characterEntity, TransformComponent& transformComponent, VelocityComponent& velocityComponent)
{
	if (Vector2Length(velocityComponent.mDirection) == 0.0f)
		return;

	velocityComponent.mNewPossiblePosition = Vector2Add(transformComponent.mPosition, Vector2Scale(Vector2Normalize(velocityComponent.mDirection), velocityComponent.mSpeed * GetFrameTime()));

}

}// namespace character	
}// namespace game
