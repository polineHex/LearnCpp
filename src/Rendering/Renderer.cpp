#include "Renderer.h"

#include <raylib.h>
#include <rlImGui.h>

#include "DebugWindow.h"
#include "Globals.h"

namespace game::Rendering
{

Renderer::Renderer(const flecs::world& ecs)
{
	InitWindow(WIDTH, HEIGHT, TITLE);
	SetTargetFPS(FPS);

	rlImGuiSetup(true);

	ecs.system("Renderer::PreDraw")
			.kind(flecs::PreStore)
			.each(PreDraw);

	ecs.system<const TransformComponent, const TextureComponent>("Renderer::DrawTextures")
			.order_by<TextureComponent>(CompareZDepth)
			.kind(flecs::OnStore)
			.each(DrawTextures);

	ecs.system<const TransformComponent, const SpriteComponent>("Renderer::DrawSprites")
			.order_by<SpriteComponent>(CompareZDepth)
			.kind(flecs::OnStore)
			.each(DrawSprites);

	ecs.system("Renderer::DrawDebug")
			.kind(flecs::PostFrame)
			.each(DrawDebug);

	ecs.system("Renderer::PostDraw")
			.kind(flecs::PostFrame)
			.each(PostDraw);
}

int Renderer::CompareZDepth(flecs::entity_t entityA, const TextureComponent* textureComponentA, flecs::entity_t entityB, const TextureComponent* textureComponentB)
{
	return textureComponentA->mZDepth - textureComponentB->mZDepth;
}

int Renderer::CompareZDepth(flecs::entity_t entityA, const SpriteComponent* spriteComponentA, flecs::entity_t entityB, const SpriteComponent* spriteComponentB)
{
	return spriteComponentA->mZDepth - spriteComponentB->mZDepth;
}
void Renderer::PreDraw(const flecs::iter& iter, int index)
{
	BeginDrawing();
	rlImGuiBegin();

	ClearBackground(WHITE);
}

void Renderer::DrawTextures(flecs::entity entity, const TransformComponent& transformComponent, const TextureComponent& textureComponent)
{
	DrawTextureV(textureComponent.mTexture, transformComponent.mPosition, WHITE);
}

void Renderer::DrawSprites(flecs::entity entity, const TransformComponent& transformComponent, const SpriteComponent& spriteComponent)
{
	DrawTexturePro(spriteComponent.mTexture, spriteComponent.mSource, {transformComponent.mPosition.x, transformComponent.mPosition.y, spriteComponent.mHeight.x, spriteComponent.mHeight.y}, spriteComponent.mOrigin, 0, WHITE);
}

void Renderer::DrawDebug(const flecs::iter& iter, int index)
{
	DrawFPS(0, 0);

	flecs::world ecs = iter.world();

	DebugWindow::Draw();

	// Keeping this commented to have it at hand easily when we need to look at the demo window.
//	ImGui::ShowDemoWindow();
}

void Renderer::PostDraw(const flecs::iter& iter, int index)
{
	rlImGuiEnd();
	EndDrawing();
}

} // namespace game::Rendering