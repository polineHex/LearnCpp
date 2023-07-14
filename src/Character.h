#include "SpriteRender.h"

namespace game
{

class Character
{
public:
	Character();
	Vector2 GetWorldPos()
	{
		return worldPos;
	}
	void SetScreenPos(int winWidth, int winHeight);
	void Update(float deltaTime);
	void Stop();

private:
	Texture2D texture{SpriteRender::LoadMyTexture("characters/knight_idle_spritesheet.png")};
	Texture2D idle{SpriteRender::LoadMyTexture("characters/knight_idle_spritesheet.png")};
	Texture2D run{SpriteRender::LoadMyTexture("characters/knight_run_spritesheet.png")};

	Vector2 screenPos{};
	Vector2 worldPos{};
	Vector2 worldPosLastFrame{};

	// 1 : facing right, -1 : facing left
	float rightLeft{1.f};

	// animation variables
	float runningTime{};         //timer to see if need to switch to next frame
	int frame{};                 //current frame
	int maxFrames{6};            //how many frames in total, can we calculate it from the texture asset?
	float updateTime{1.f / 12.f};//when do we switch to the next frame, anim is 12 frames per sec
	float speed{4.f};            //running speed

	//variables of the frame, to set in constructor
	float width{};
	float height{};
};

}// namespace game
