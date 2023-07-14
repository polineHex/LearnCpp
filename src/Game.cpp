#include "Game.h"

#include <raylib.h>
#include <raymath.h>

#include "Map/Map.h"

namespace game
{

Game::Game() : mRenderer(mEcs)
{
	Map::CreateMap(mEcs);
	Character::InitCharacter(mEcs);
}

void Game::Start()
{
	while (0)
	{
		Input();
		Update();
		Draw();
	}

	while (mEcs.progress(GetFrameTime()))
	{
		// There shouldn't be anything in here.
		// It should all be systems.
	}
}

} // namespace game