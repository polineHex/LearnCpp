#include "RenderUtils.h"

namespace game
{

Texture2D RenderUtils::LoadMyTexture(const std::string& texturePath)
{
	std::string basePath{};
#if defined(_WIN32) || defined(_WIN64)
	basePath = "../../../";
#elif __APPLE__
	basePath = "../";
#endif

	const std::string fullPath = basePath + "assets/textures/" + texturePath;

	Texture2D texture = LoadTexture(fullPath.c_str());
	return texture;
}

}// namespace game