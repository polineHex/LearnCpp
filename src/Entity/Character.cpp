#include "Character.h"

#include <raylib.h>
#include <raymath.h>

#include "Globals.h"

#include "Enemy.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/CoinsComponent.h"
#include "Entity/Components/HealthComponent.h"

#include "Rendering/RenderUtils.h"

#include "Rendering/Components/AnimationComponent.h"
#include "Rendering/Components/AnimationStateComponent.h"
#include "Rendering/Components/SpriteComponent.h"

#include "Coin.h"
#include "Physics/Components/CollisionComponent.h"
#include "Physics/Components/VelocityComponent.h"


namespace game::character
{
static Vector2 gCharacterSize{CHARACTER_WIDTH * ENTITY_SCALE, CHARACTER_HEIGHT * ENTITY_SCALE};
//TODO:Calculate it from width later
static const int gCoinPickupRadius{20};
//Dependant on the weapon type?
static const int gAttackRadius{50};

static void CharacterInput(flecs::entity characterEntity, VelocityComponent& velocityComponent, AnimationStateComponent& animationStateComponent, AnimationComponent& animationComponent);
static void CharacterUpdate(flecs::entity characterEntity, TransformComponent& transformComponent, VelocityComponent& velocityComponent);
static void PickCoins(flecs::entity characterEntity, CoinsComponent& coinsComponent, const TransformComponent& transformComponent);
static void SpawnCoin(flecs::world& ecs, const Vector2 position);
static void Attack(flecs::entity& characterEntity);

void InitCharacter(flecs::world& ecs)
{
	const TransformComponent transformComponent{CHARACTER_SPAWN_POINT, gCharacterSize};

	flecs::entity playerEntity = ecs.entity("Player")
									.add<CharacterTag>()
									.emplace<CoinsComponent>(100, 10)
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
	ecs.system<VelocityComponent, AnimationStateComponent, AnimationComponent>("CharacterInput")
			.with<CharacterTag>()
			.kind(flecs::PreUpdate)
			.each(CharacterInput);

	//Logic on update
	ecs.system<TransformComponent, VelocityComponent>("CharacterUpdate")
			.with<CharacterTag>()
			.kind(flecs::PreUpdate)
			.each(CharacterUpdate);

	ecs.system<CoinsComponent, const TransformComponent>("PickCoins")
			.with<CharacterTag>()
			.kind(flecs::OnUpdate)
			.each(PickCoins);

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
	const Vector2 playerPosition = characterEntity.get_ref<TransformComponent>()->mPosition;

	flecs::world ecs = characterEntity.world();
	auto filter = ecs.filter_builder<TransformComponent, HealthComponent>().with<EnemyTag>().build();

	filter.each([&playerPosition, &ecs](flecs::entity e, TransformComponent& transformComponent, HealthComponent& enemyHealth) {

		if (Vector2Distance(transformComponent.mPosition, playerPosition) <= gAttackRadius){
			enemyHealth.mCurrentHealth -= 50;

			if (enemyHealth.mCurrentHealth <= 0) {
				SpawnCoin(ecs, transformComponent.mPosition);
				e.destruct();
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

void SpawnCoin(flecs::world& ecs, const Vector2 position) {

	static int index = 0;
	const std::string coinName = std::string("coin" + std::to_string(index++));

	ecs.entity(coinName.c_str())
			.is_a<CoinPrefab>()
			.set<TransformComponent>({position});

}

void PickCoins(flecs::entity characterEntity, CoinsComponent& coinsComponent, const TransformComponent& transformComponent)
{
	const Vector2 playerPosition = transformComponent.mPosition;
	int newCoinsAmount = coinsComponent.mCoinAmount;

	flecs::world ecs = characterEntity.world();
	auto filter = ecs.filter_builder<TransformComponent>().with<CoinTag>().build();

	filter.each([&newCoinsAmount, &playerPosition](flecs::entity e, TransformComponent& transformComponent) {

		if (Vector2Distance(transformComponent.mPosition, playerPosition) <= gCoinPickupRadius){
			newCoinsAmount++;
			e.destruct();
		}
	});

	if (newCoinsAmount >= coinsComponent.mMaxCoinAmount)
		newCoinsAmount = coinsComponent.mCoinAmount;

	coinsComponent.mCoinAmount = newCoinsAmount;
}

} // namespace game::character

