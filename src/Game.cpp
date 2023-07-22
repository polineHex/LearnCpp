#include "Game.h"

#include <raylib.h>

#include "Map/Map.h"
#include "Entity/Character.h"
#include "Entity/Tower.h"

namespace game
{

Game::Game() : mRenderer(mEcs)
{
	map::CreateMap(mEcs);
	Character::InitCharacter(mEcs);
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