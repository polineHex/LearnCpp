#include "Coin.h"

#include "Globals.h"

#include "Entity/Components/TransformComponent.h"
#include "Physics/Components/CollisionComponent.h"
#include "Rendering/Components/AnimationComponent.h"
#include "Rendering/Components/AnimationStateComponent.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/RenderUtils.h"

namespace game::coin
{
static Vector2 gCoinSize{COIN_WIDTH * ENTITY_SCALE, COIN_HEIGHT * ENTITY_SCALE};

static void SpawnCoin(flecs::iter& iter);

void InitCoin(flecs::world& ecs)
{
	ecs.prefab<CoinPrefab>("coinPrefab")
			.add<CoinTag>()
			.override<TransformComponent>()
			.emplace<CollisionComponent>(gCoinSize)
			.set_override<SpriteComponent>({renderUtils::LoadMyTexture("items/coin_spritesheet_16x16.png"),
											{0, 0, COIN_WIDTH, COIN_HEIGHT},
											{gCoinSize.x, gCoinSize.y},
											{0, 0},
											1})
			.emplace_override<AnimationComponent>(5, 0.1f, 0.0f, 0)
			.emplace<AnimationStateComponent>(AnimationName::IDLE, false);;

	ecs.system("SpawnCoin")
			.kind(flecs::OnUpdate)
			.iter(SpawnCoin);
}

void SpawnCoin(flecs::iter& iter)
{
	if (!IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
		return;

	const TransformComponent transformComponent{GetMousePosition(), gCoinSize};

	static int index = 0;
	const std::string coinName = std::string("coin" + std::to_string(index++));

	iter.world().entity(coinName.c_str())
			.is_a<CoinPrefab>()
			.set<TransformComponent>(transformComponent);
}


} // namespace game::coin

