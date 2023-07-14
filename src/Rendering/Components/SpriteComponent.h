#pragma once

#include <raylib.h>

struct SpriteComponent
{
	Texture2D mTexture;
	Rectangle mSource;
	Vector2 mHeight;
	Vector2 mOrigin;
	int mZDepth;
};