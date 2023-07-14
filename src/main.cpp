#include <iostream>

#include "Game.h"

struct PositionComponent
{
	int x;
	int y;
	int z;
};

void MovePosition(flecs::entity entity, PositionComponent& positionComponent)
{
	std::cout << entity.name();
}

int main()
{
	game::Game game{};
	game.Start();

	return 0;
}
