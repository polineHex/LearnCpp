#pragma once

#include <raylib.h>

namespace game
{

 enum class AnimationName
{
	IDLE,//QUESTION: google style says it should be kIdle?
	RUN,
	ATTACK,
};

struct AnimationStateComponent {
	AnimationName mCurrentAnimName;
};

}// namespace game