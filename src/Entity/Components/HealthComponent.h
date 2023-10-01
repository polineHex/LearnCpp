#pragma once

namespace game
{

struct HealthComponent
{
	int mMaxHealth{100};
	int mCurrentHealth{mMaxHealth};

};

}// namespace game