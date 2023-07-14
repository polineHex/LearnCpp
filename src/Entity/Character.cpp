#include "Character.h"

#include <raymath.h>

#include "Entity/Components/TransformComponent.h"
#include "Rendering/Components/SpriteComponent.h"

namespace game
{

void Character::InitCharacter(flecs::world& ecs)
{
	ecs.entity("Player")
			.add<CharacterTag>()
			.set<TransformComponent>({100, 100})
			.set<SpriteComponent>({RenderUtils::LoadMyTexture("characters/knight_idle_spritesheet.png"), {0, 0, CHARACTER_WIDTH, CHARACTER_HEIGHT}, {CHARACTER_WIDTH * 2, CHARACTER_HEIGHT * 2}, {0, 0}, 0});

	ecs.system<TransformComponent>()
			.with<CharacterTag>()
			.kind(flecs::OnUpdate)
			.each(CharacterUpdate);
}

void Character::CharacterUpdate(flecs::entity characterEntity, TransformComponent& transformComponent)
{
	Vector2 direction{};
	if (IsKeyDown(KEY_A))
		direction.x -= 1.0;
	else if (IsKeyDown(KEY_D))
		direction.x += 1.0;

	if (IsKeyDown(KEY_W))
		direction.y -= 1.0;
	else if (IsKeyDown(KEY_S))
		direction.y += 1.0;

	if (Vector2Length(direction) == 0.0)
		return;

	direction = Vector2Normalize(direction);
	transformComponent.mPosition = Vector2Add(transformComponent.mPosition, Vector2Scale(Vector2Normalize(direction), CHARACTER_SPEED));

	// TODO: Check for map bounds.
}

}// namespace game