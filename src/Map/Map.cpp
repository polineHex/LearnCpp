#include "Map.h"

#include "Entity/Components/TransformComponent.h"
#include "Rendering/RenderUtils.h"
#include "Rendering/Components/TextureComponent.h"


namespace game
{

namespace map
{

void CreateMap(flecs::world& ecs)
{
	const flecs::entity mapEntity = ecs.entity("Map")
		.set<TransformComponent>({0, 0})
		.emplace<TextureComponent>(renderUtils::LoadMyTexture("map/map.png"), 0);

	// Add a pair on the flecs world (MapTag, mapEntity).
	ecs.add<MapTag>(mapEntity);
}

bool IsWithinMapBounds(const flecs::entity& mapEntity, const Vector2 newPosition, const Vector2 collisionSize)
{

	//Check for map bounds
	const Texture2D mapTexture = mapEntity.get_ref<TextureComponent>()->mTexture;

	return (newPosition.x >= 0 && newPosition.x <= mapTexture.width - collisionSize.x &&
			newPosition.y >= 0 && newPosition.y <= mapTexture.height - collisionSize.y);
}

} // namespace map

}// namespace game