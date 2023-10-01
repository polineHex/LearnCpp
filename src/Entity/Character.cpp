#include "Character.h"

#include <raylib.h>
#include <raymath.h>

#include "Globals.h"
#include "Map/Map.h"

#include "Enemy.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/HealthComponent.h"

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

static void CharacterInput(flecs::entity characterEntity, VelocityComponent& velocityComponent, AnimationStateComponent& animationStateComponent, AnimationComponent& animationComponent);
static void CharacterUpdate(flecs::entity characterEntity, TransformComponent& transformComponent, VelocityComponent& velocityComponent);
static void Attack(flecs::entity& characterEntity);

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
									.set<AnimationStateComponent>({AnimationName::IDLE, false});

	ecs.add<CharacterTag>(playerEntity);

	//Order is important. Input is before update
	ecs.system<VelocityComponent, AnimationStateComponent, AnimationComponent>()
			.with<CharacterTag>()
			.kind(flecs::PreUpdate)
			.each(CharacterInput);

	//Logic on update
	ecs.system<TransformComponent, VelocityComponent>()
			.with<CharacterTag>()
			.kind(flecs::PreUpdate)
			.each(CharacterUpdate);

	//Rendering is after logic is done e.g. PreStore/PostUpdate
}

void CharacterInput(flecs::entity characterEntity, VelocityComponent& velocityComponent, AnimationStateComponent& animationStateComponent, AnimationComponent& animationComponent)
{
	if (animationStateComponent.mIsAttacking)
		return;

	velocityComponent.mPrevDirection = velocityComponent.mDirection;
	velocityComponent.mDirection = {0, 0};

	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		animationStateComponent.mIsAttacking = true;
		animationComponent.mCurrentFrame = 0; //To start attack from first frame
		Attack(characterEntity);
		return;
	}

	if (IsKeyDown(KEY_A))
		velocityComponent.mDirection.x -= 1.0f;
	else if (IsKeyDown(KEY_D))
		velocityComponent.mDirection.x += 1.0f;

	if (IsKeyDown(KEY_W))
		velocityComponent.mDirection.y -= 1.0f;
	else if (IsKeyDown(KEY_S))
		velocityComponent.mDirection.y += 1.0f;


}

void Attack(flecs::entity& characterEntity)
{
	const Vector2 playerCollision = characterEntity.get<CollisionComponent>()->mRectScale;
	const Vector2 playerPosition = characterEntity.get_ref<TransformComponent>()->mPosition;
	const Rectangle playerRect{playerPosition.x, playerPosition.y,playerCollision.x, playerCollision.y};

	flecs::world ecs = characterEntity.world();
	auto filter = ecs.filter_builder<TransformComponent, CollisionComponent, HealthComponent>().with<EnemyTag>().build();

	filter.each([&playerRect](flecs::entity e, TransformComponent& transformComponent, CollisionComponent& collisionComponent, HealthComponent& enemyHealth) {
		// Define enemy rectangle
		const Rectangle enemyRect{transformComponent.mPosition.x - 10, transformComponent.mPosition.y - 10, collisionComponent.mRectScale.x + 20, collisionComponent.mRectScale.y + 20};

		// Check if position intersects with enemy
		if (CheckCollisionRecs(playerRect, enemyRect)) {
			// Deal damage to the enemy
			enemyHealth.mCurrentHealth -= 50;

			// Check if enemy's health is depleted
			if (enemyHealth.mCurrentHealth <= 0) {
				e.destruct();  // Remove the enemy entity
			}
		}
	});

}

void CharacterUpdate(flecs::entity characterEntity, TransformComponent& transformComponent, VelocityComponent& velocityComponent)
{
	if (Vector2Length(velocityComponent.mDirection) == 0.0f)
		return;

	velocityComponent.mNewPossiblePosition = Vector2Add(transformComponent.mPosition, Vector2Scale(Vector2Normalize(velocityComponent.mDirection), velocityComponent.mSpeed * GetFrameTime()));

}

}// namespace character	
}// namespace game
