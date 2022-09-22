#pragma once

#include "../../core.h"
#include "material_handler.h"
#include "../../core/ecs/actor.h"

namespace Shard3D {
	namespace Components {
		struct TransformComponent;
		struct Mesh3DComponent;
	}
	class RenderHandler {
	public:
		static void addActorToRenderList(ECS::Actor actor);
		static void refreshActor(const ECS::Actor& actor);
		static SurfaceMaterialRenderInfo buildRenderInfoFromActor(ECS::Actor actor, SurfaceMaterialClassOptionsFlags flags);
		static std::vector<SurfaceMaterialClassOptionsFlags> getRenderUsingClasses(UUID actorID);
		static void clear();

		static void addMesh3DComponentToActor(ECS::Actor actor, AssetID mesh);
		static void rmvMesh3DComponentFromActor(ECS::Actor actor);
		static void modifyMaterialType(AssetID material, SurfaceMaterialClassOptionsFlags old_flags, SurfaceMaterialClassOptionsFlags new_flags);

		static void addToSurfaceMaterialRenderingList(UUID actorID, SurfaceMaterialClassOptionsFlags flags, const SurfaceMaterialRenderInfo& renderInfo);
		static void rmvFromSurfaceMaterialRenderingList(UUID actorID, SurfaceMaterialClassOptionsFlags flags);
		static void switchSurfaceMaterialRenderingList(UUID actorID, SurfaceMaterialClassOptionsFlags old_flags, SurfaceMaterialClassOptionsFlags new_flags, const SurfaceMaterialRenderInfo& new_renderInfo);
		static auto& getSurfaceMaterialRenderingList() { return materialRendering; }
	private:
		friend class MaterialHandler;
		static hashMap<SurfaceMaterialClassOptionsFlags, hashMap<UUID, SurfaceMaterialRenderInfo>> materialRendering;
	};
}
