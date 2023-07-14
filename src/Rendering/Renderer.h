#pragma once

#include <flecs.h>

#include "Entity/Components/TransformComponent.h"
#include "Rendering/Components/SpriteComponent.h"
#include "Rendering/Components/TextureComponent.h"

namespace game::Rendering
{

class Renderer
{
public:
	explicit Renderer(const flecs::world& ecs);

private:
	static void PreDraw(const flecs::iter& iter, int index);
	static int CompareZDepth(flecs::entity_t entityA, const TextureComponent* textureComponentA, flecs::entity_t entityB, const TextureComponent* textureComponentB);
	static int CompareZDepth(flecs::entity_t entityA, const SpriteComponent* spriteComponentA, flecs::entity_t entityB, const SpriteComponent* spriteComponentB);
	static void DrawTextures(flecs::entity entity, const TransformComponent& transformComponent, const TextureComponent& textureComponent);
	static void DrawSprites(flecs::entity entity, const TransformComponent& transformComponent, const SpriteComponent& spriteComponent);
	static void DrawDebug(const flecs::iter& iter, int index);
	static void PostDraw(const flecs::iter& iter, int index);
};

} // namespace game::Rendering