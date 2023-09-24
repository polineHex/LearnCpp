#pragma once

#include <raylib.h>

namespace game
{

struct HealthComponent
{
	int mMaxHealth{100};
	int mCurrentHealth{mMaxHealth};

};

}// namespace game