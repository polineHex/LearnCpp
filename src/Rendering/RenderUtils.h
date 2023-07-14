#pragma once

#include <string>
#include <raylib.h>

namespace game
{

class RenderUtils
{
public:
	static Texture2D LoadMyTexture(std::string texturePath);
};

}// namespace game