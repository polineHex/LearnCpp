#pragma once

#include <raylib.h>

namespace game
{

struct SpriteComponent
{
	Texture2D mTexture;
	Rectangle mSource;
	Vector2 mHeight;
	Vector2 mOrigin;
	int mZDepth;
};
}// namespace game