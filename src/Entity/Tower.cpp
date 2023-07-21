#include "Tower.h"

#include "Rendering/RenderUtils.h"

#include "Entity/Components/TransformComponent.h"
#include "Physics/Components/CollisionComponent.h"
#include "Physics/PhysicsUtils.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/Components/AnimationComponent.h"

namespace game
{

void Tower::InitTower(flecs::world& ecs)
{
	// Register components QUESTION:why we didnt have to do it before?
	ecs.component<CollisionComponent>();

	ecs.system<>("HandleMouseClick")
			.kind(flecs::OnUpdate)
			.iter([&ecs](flecs::iter) //TODO: look up how this exactly works and why
			{
				if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
				{
					Vector2 mousePosition = GetMousePosition();
					int spriteIndex = GetRandomValue(0, 3);
					
					//it doesnt account for the scale, hardcoding it really starts to bite me in the ass - redo asap
					Rectangle newTowerRect = {mousePosition.x, mousePosition.y, TOWER_WIDTH*2, TOWER_HEIGHT*2};
					bool isCollided = false;

					ecs.each<CollisionComponent>([&](flecs::entity e, CollisionComponent& collisionComponent) {
						if (PhysicsUtils::CheckCollisionRecs(newTowerRect, collisionComponent.mCollisionRect))
						{
							//return;
							isCollided = true;

						}
					});
					
					//If no collision was detected, we create a tower 
					//TODO:redo collision component to only have dimension and use transform as a starting point of a rect
					//it doesnt account for the scale, hardcoding it really starts to bite me in the ass - redo asap!!

					if (!isCollided)
					{
						ecs.entity()
								.add<TowerTag>()
								.set<TransformComponent>({mousePosition.x, mousePosition.y})
								.set<CollisionComponent>({mousePosition.x, mousePosition.y, TOWER_WIDTH*2, TOWER_HEIGHT*2})
								.set<SpriteComponent>({RenderUtils::LoadMyTexture("buildings/towers_spritesheet_16x32_4x1.png"), 
													{(float)spriteIndex * TOWER_WIDTH, 0, TOWER_WIDTH, TOWER_HEIGHT}, 
													{TOWER_WIDTH * 2, TOWER_HEIGHT * 2}, {0, 0}, 0});
					}
				}
			});
}



}// namespace game

