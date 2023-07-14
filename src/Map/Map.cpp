#include "Map.h"

#include "Rendering/RenderUtils.h"
#include "Entity/Components/TransformComponent.h"
#include "Rendering/Components/TextureComponent.h"

namespace game
{

void Map::CreateMap(flecs::world& ecs)
{
	ecs.entity("Map")
		.set<TransformComponent>({0, 0})
		.emplace<TextureComponent>(RenderUtils::LoadMyTexture("map/map.png"), 0);
}

} // namespace game