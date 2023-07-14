#include "RenderUtils.h"

namespace game
{

Texture2D RenderUtils::LoadMyTexture(std::string texturePath)
{
	std::string fullPath = "../../../assets/textures/" + texturePath;
	Texture2D texture = LoadTexture(fullPath.c_str());
	return texture;
}

}// namespace game