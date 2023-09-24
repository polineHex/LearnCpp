#include "Renderer.h"

#include <math.h>

#include <raylib.h>
#include <rlImGui.h>

#include "DebugWindow.h"
#include "Globals.h"

namespace game
{
namespace Rendering
{

Renderer::Renderer(const flecs::world& ecs)
{
	InitWindow(WIDTH, HEIGHT, TITLE);
	SetTargetFPS(gFps);

	rlImGuiSetup(true);

	ecs.system("Renderer::PreDraw")
			.kind(flecs::PreStore)
			.each(PreDraw);


	//QUESTION: had to change it to OnUpdate cause map was drawn over rectangles
	ecs.system<const TransformComponent, const TextureComponent>("Renderer::DrawTextures")
			.order_by<TextureComponent>(CompareZDepth)
			.kind(flecs::OnUpdate)
			.each(DrawTextures);


	ecs.system<const TransformComponent,const SpriteComponent>("Renderer::DrawSprites")
			.order_by<SpriteComponent>(CompareZDepth)
			.kind(flecs::OnStore)
			.each(DrawSprites);

	ecs.system<AnimationComponent, SpriteComponent, AnimationStateComponent>()
			.kind(flecs::OnUpdate)
			.each(UpdateAnimationFrame);


	ecs.system<const TransformComponent, const CollisionComponent, const HealthComponent>()
			.kind(flecs::PostUpdate)
			.each(UpdateHealthBar);

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
	DrawTexturePro(spriteComponent.mTexture,
				   {spriteComponent.mSource.x, spriteComponent.mSource.y, copysignf(spriteComponent.mSource.width, transformComponent.mScale.x), spriteComponent.mSource.height},
				   {transformComponent.mPosition.x, transformComponent.mPosition.y, spriteComponent.mHeight.x, spriteComponent.mHeight.y},
				   spriteComponent.mOrigin, 0, WHITE);
}

void Renderer::UpdateAnimationFrame(flecs::entity entity, AnimationComponent& animationComponent, SpriteComponent& spriteComponent, AnimationStateComponent& animationStateComponent)
{
	animationComponent.mTimer += GetFrameTime();
	spriteComponent.mSource.y = (int)animationStateComponent.mCurrentAnimName * spriteComponent.mSource.height;

	if (animationComponent.mTimer >= animationComponent.mFrameDuration)
	{
		animationComponent.mCurrentFrame++;
		animationComponent.mTimer = 0.f;

		if (animationComponent.mCurrentFrame > animationComponent.mFrameCount)
		{
			animationComponent.mCurrentFrame = 0;
			//Don't loop attack animations
			if (animationStateComponent.mIsAttacking)
				animationStateComponent.mIsAttacking = false;

		}
		
		spriteComponent.mSource.x = animationComponent.mCurrentFrame * spriteComponent.mSource.width;
	}
}

void Renderer::UpdateHealthBar(flecs::entity entity, const TransformComponent& transformComponent, const CollisionComponent& collisionComponent, const HealthComponent& healthComponent)
{
	Vector2 healthBarPosition{transformComponent.mPosition.x, transformComponent.mPosition.y - 10}; // 10 units above the entity
	float healthBarWidth{collisionComponent.mRectScale.x};
	float healthBarHeight{10};

	// Draw background of health bar (black rectangle)
	DrawRectangle(healthBarPosition.x, healthBarPosition.y, healthBarWidth, healthBarHeight, BLACK);

	// Draw collision rectangle (red rectangle)
	DrawRectangle(transformComponent.mPosition.x - 10, transformComponent.mPosition.y - 10, collisionComponent.mRectScale.x + 20, collisionComponent.mRectScale.y + 20, RED);

	// Calculate width of the green health bar based on current health
	float healthPercentage = static_cast<float>(healthComponent.mCurrentHealth) / healthComponent.mMaxHealth;
	float greenBarWidth = healthBarWidth * healthPercentage;

	// Draw green health rectangle
	DrawRectangle(healthBarPosition.x, healthBarPosition.y, greenBarWidth, healthBarHeight, GREEN);
}

void Renderer::DrawDebug(const flecs::iter& iter, int index)
{
	DrawFPS(0, 0);

	flecs::world ecs = iter.world();

	DebugWindow::Draw(ecs);

	// Keeping this commented to have it at hand easily when we need to look at the demo window.
	//	ImGui::ShowDemoWindow();
}

void Renderer::PostDraw(const flecs::iter& iter, int index)
{
	rlImGuiEnd();
	EndDrawing();
}

}// namespace Rendering	
} // namespace game