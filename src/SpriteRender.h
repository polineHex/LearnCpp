#include <string>
#include <raylib.h>

namespace game
{

class SpriteRender
{
public:
	static Texture2D LoadMyTexture(std::string texturePath);
};

}// namespace game