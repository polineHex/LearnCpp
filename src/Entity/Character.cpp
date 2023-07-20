#include "Character.h"

#include <raymath.h>

#include "Entity/Components/TransformComponent.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/Components/AnimationComponent.h"

namespace game
{

void Character::InitCharacter(flecs::world& ecs)
{
	ecs.entity("Player")
			.add<CharacterTag>()
			.set<TransformComponent>({100, 100})
			.set<SpriteComponent>({RenderUtils::LoadMyTexture("characters/knight_spritesheet_16x16_8x2.png"), {0, 0, CHARACTER_WIDTH, CHARACTER_HEIGHT}, {CHARACTER_WIDTH * 2, CHARACTER_HEIGHT * 2}, {0, 0}, 0})
			.emplace<AnimationComponent>(5, 0.1f, 0.0f, 0)
			.emplace<AnimationStateComponent>(AnimationName::IDLE);
	ecs.system<TransformComponent, AnimationStateComponent>()
			.with<CharacterTag>()
			.kind(flecs::OnUpdate)
			.each(CharacterUpdate);
}

//can i take a texture from sprite component here and get the width?
//is there a way to show an entity with the list of attached components in debug?
//TODO different anims? how is sprite component then initialized? with idle at start and then i need to set sprite component in Update 
//character? (when we realaize we move i.e. transform changes)

void Character::CharacterUpdate(flecs::entity characterEntity, TransformComponent& transformComponent, AnimationStateComponent& animationStateComponent)
{
	Vector2 direction{};
	int frame{};

	if (IsKeyDown(KEY_A))
		direction.x -= 1.0;
	else if (IsKeyDown(KEY_D))
		direction.x += 1.0;

	if (IsKeyDown(KEY_W))
		direction.y -= 1.0;
	else if (IsKeyDown(KEY_S))
		direction.y += 1.0;

	if (Vector2Length(direction) == 0.0)
	{
		animationStateComponent.mCurrentAnimName = AnimationName::IDLE;
		return;
	}
		
	//changing animation row (aka switching to different animation) when running
	animationStateComponent.mCurrentAnimName = AnimationName::RUN;
	
	//changing orientation of the character depending on where it's moving
	direction.x < 0.f ? transformComponent.mScale.x = -1.f : transformComponent.mScale.x = 1.f;
	transformComponent.mPosition = Vector2Add(transformComponent.mPosition, Vector2Scale(Vector2Normalize(direction), CHARACTER_SPEED));




	// TODO: Check for map bounds.
}

}// namespace game