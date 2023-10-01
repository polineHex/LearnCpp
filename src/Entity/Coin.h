#pragma once

#include <flecs.h>


namespace game
{

const int COIN_WIDTH = 16;
const int COIN_HEIGHT = 16;

struct CoinTag {};
struct CoinPrefab {};

namespace coin
{


void InitCoin(flecs::world& ecs);


}//namespace coin
}// namespace game