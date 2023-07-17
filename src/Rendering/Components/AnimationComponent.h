#pragma once

#include <raylib.h>

namespace game
{

struct AnimationComponent {
	int mFrameCount{};     
	float mFrameDuration{};
	
	// Timer for frame updates
	float mTimer{};        
	int mCurrentFrame{};
};
}// namespace game