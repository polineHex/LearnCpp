#include "Map.h"

#include "Rendering/RenderUtils.h"
#include "Rendering/Components/TextureComponent.h"
#include "Entity/Components/TransformComponent.h"


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

} // namespace map

}// namespace game