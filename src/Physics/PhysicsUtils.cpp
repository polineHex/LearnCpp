#include "PhysicsUtils.h"

namespace game
{

 bool PhysicsUtils::CheckCollisionRecs(Rectangle rec1, Rectangle rec2)
{
	return !(rec2.x >= (rec1.x + rec1.width) || (rec2.x + rec2.width) <= rec1.x || rec2.y >= (rec1.y + rec1.height) || (rec2.y + rec2.height) <= rec1.y);
}

}// namespace game