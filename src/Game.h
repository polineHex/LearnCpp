#pragma once

#include <flecs.h>
#include "SpriteRender.h"
#include "Character.h"
#include "Globals.h"

#include "Rendering/Renderer.h"

namespace game
{

class Game
{
public:
	explicit Game();
	void Start();

private:
	flecs::world mEcs{};
	Rendering::Renderer mRenderer;

	Texture2D map{SpriteRender::LoadMyTexture("map/map.png")};//can i leave it empty?
	Vector2 mapPos{0.0, 0.0};
	Character hero; //do i need to do hero{} if i have a constructor?
};

} // namespace game