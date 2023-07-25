#include "RenderUtils.h"

#include <unordered_map>

namespace game
{

namespace renderUtils
{

std::unordered_map<std::string, Texture2D> textureMap{};

Texture2D LoadMyTexture(const std::string& texturePath)
{
	if (textureMap.find(texturePath) != textureMap.end())
		return textureMap[texturePath];

	std::string basePath{};
#if defined(_WIN32) || defined(_WIN64)
	basePath = "../../../";
#elif __APPLE__
	basePath = "../";
#endif

	const std::string fullPath = basePath + "assets/textures/" + texturePath;

	Texture2D texture = LoadTexture(fullPath.c_str());
	textureMap[texturePath] = texture;

	return texture;
}

}// namespace renderUtils
}// namespace game