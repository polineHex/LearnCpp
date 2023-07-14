#include "Game.h"
#include "raymath.h"
#include <raylib.h>

namespace game
{

Game::Game() : mRenderer(mEcs)
{
}

void Game::Start()
{
	while (mEcs.progress(GetFrameTime()))
	{

		DrawTextureEx(map, mapPos, 0.0, 1.0, WHITE);
		//hero.Update(GetFrameTime());
		//// check map bounds
		//if (hero.GetWorldPos().x < 0.f ||
		//	hero.GetWorldPos().y < 0.f ||
		//	hero.GetWorldPos().x + WIDTH > map.width * mapScale ||
		//	hero.GetWorldPos().y + HEIGHT > map.height * mapScale)
		//{
		//	hero.Stop();
		//}



	}
}

} // namespace game