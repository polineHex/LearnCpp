#pragma once

#include <raylib.h>

namespace game
{

 enum class AnimationName
{
	IDLE,
	RUN,
	ATTACK,
};

struct AnimationStateComponent {
	AnimationName mCurrentAnimName;
	bool mIsAttacking{false};
};

}// namespace game