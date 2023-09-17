#include "Game.h"

#include <raylib.h>

#include "Map/Map.h"
#include "Entity/Entity.h"
#include "Entity/Tower.h"

#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/TargetTowerComponent.h"

#include "Rendering/RenderUtils.h"

#include "Rendering/Components/AnimationComponent.h"
#include "Rendering/Components/AnimationStateComponent.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/Components/TextureComponent.h"

#include "Physics/Components/CollisionComponent.h"
#include "Physics/Components/VelocityComponent.h"

namespace game
{

static void RegisterComponents(const flecs::world& ecs)
{
	// 3rd-Party components/structs (e.g. raylib).
	ecs.component<Rectangle>()
			.member<float>("x")
			.member<float>("y")
			.member<float>("width")
			.member<float>("height");

	ecs.component<Vector2>()
			.member<float>("x")
			.member<float>("y");

	ecs.component<Texture>()
			.member<unsigned int>("id")
			.member<int>("width")
			.member<int>("height")
			.member<int>("mipmaps")
			.member<int>("format");

	// Game components/structs.

	ecs.component<CollisionComponent>()
			.member<Vector2>("mRectScale");
	ecs.component<VelocityComponent>()
			.member<Vector2>("mDirection")
			.member<float>("mSpeed");

	ecs.component<TransformComponent>()
			.member<Vector2>("mPosition")
			.member<Vector2>("mScale");

	ecs.component<TargetTowerComponent>()
			.member<flecs::entity_t>("mTowerTarget");

	ecs.component<SpriteComponent>()
			.member<Texture2D>("mTexture")
			.member<Rectangle>("mSource")
			.member<Vector2>("mHeight")
			.member<Vector2>("mOrigin")
			.member<int>("mZDepth");

	ecs.component<AnimationComponent>()
			.member<int>("mFrameCount")
			.member<float>("mFrameDuration")
			.member<float>("mTimer")
			.member<int>("mCurrentFrame");

	ecs.component<AnimationName>()
			.constant("IDLE", AnimationName::IDLE)
			.constant("RUN", AnimationName::RUN)
			.constant("ATTACK", AnimationName::ATTACK);

	ecs.component<AnimationStateComponent>()
	        .member<AnimationName>("mCurrentAnimName");
}

Game::Game() : mRenderer(mEcs)
{
	RegisterComponents(mEcs);

	map::CreateMap(mEcs);
	entity::InitEntity(mEcs);
	Tower::InitTower(mEcs);

	mEcs.set<flecs::Rest>({});
	flecs::log::set_level(0);
	mEcs.import<flecs::monitor>();
}

void Game::Start()
{
	while (mEcs.progress(GetFrameTime()))
	{
		if (WindowShouldClose())
			mEcs.quit();
		// There shouldn't be anything in here.
		// It should all be systems.
	}
}

} // namespace game