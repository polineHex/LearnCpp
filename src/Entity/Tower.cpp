#include "Tower.h"

#include <raymath.h>

#include "Rendering/RenderUtils.h"

namespace game
{

void Tower::InitTower(flecs::world& ecs)
{
	ecs.system<>("HandleMouseClick")
			.kind(flecs::OnUpdate)
			.iter([&ecs](flecs::iter)
			{
				if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
				{
					Vector2 mousePosition = GetMousePosition();
					int spriteIndex = GetRandomValue(0, 3);
					ecs.entity()
							.add<TowerTag>()
							.set<TransformComponent>({mousePosition.x, mousePosition.y})
							.set<SpriteComponent>({RenderUtils::LoadMyTexture("buildings/towers_spritesheet_16x32_4x1.png"), {(float)spriteIndex * TOWER_WIDTH, 0, TOWER_WIDTH, TOWER_HEIGHT}, {TOWER_WIDTH * 2, TOWER_HEIGHT * 2}, {0, 0}, 0});
				}
			});
}


}// namespace game

