#pragma once

#include <raylib.h>

namespace game
{

struct TextureComponent
{
	Texture2D mTexture;
	int mZDepth;
};

} // namespace game