#pragma once

#include <flecs.h>

#include "Globals.h"
#include "Rendering/RenderUtils.h"
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
};

} // namespace game