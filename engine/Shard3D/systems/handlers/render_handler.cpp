#include "../../s3dpch.h" 

#include "render_handler.h"
#include "../../core/asset/assetmgr.h"
#include <fstream>
#include "../computational/shader_system.h"
namespace Shard3D {
	hashMap<SurfaceMaterialClassOptionsFlags, hashMap<UUID, SurfaceMaterialRenderInfo>> RenderHandler::materialRendering;

	void RenderHandler::addActorToRenderList(ECS::Actor actor) {
		for (SurfaceMaterialClassOptionsFlags class_ : MaterialHandler::getAvailableClasses()) {
			addToSurfaceMaterialRenderingList(actor.getUUID(), class_, buildRenderInfoFromActor(actor, class_));
		}
	}
	void RenderHandler::modifyMaterialType(AssetID material, SurfaceMaterialClassOptionsFlags old_flags, SurfaceMaterialClassOptionsFlags new_flags) {
		std::vector<UUID> outdated_actors{};
		
		for (auto& [actor, old_renderInfo] : materialRendering[old_flags]) {
			SHARD3D_LOG("checking actor {0}", actor);
			for (size_t i = 0; i < old_renderInfo.material_indices.size(); i++) {
				uint32_t mxIndex = old_renderInfo.material_indices[i];
				if (old_renderInfo.mesh->materials[mxIndex] != material) continue;
				VectorUtils::eraseItemAtIndex(old_renderInfo.material_indices, i);
				materialRendering[new_flags][actor].material_indices.push_back(mxIndex);
				materialRendering[new_flags][actor].mesh = old_renderInfo.mesh;
				materialRendering[new_flags][actor].transform = old_renderInfo.transform;
				SHARD3D_LOG("Replaced material data from old class ({0}) to new ({1})", old_flags, new_flags);
			}
			if (old_renderInfo.material_indices.empty()) {
				outdated_actors.push_back(actor);
				SHARD3D_LOG("Removed material {0} from Actor {1} since references have been destroyed", material.getFile(), actor);
			}
		}

		for (UUID& actor : outdated_actors) {
			rmvFromSurfaceMaterialRenderingList(actor, old_flags);
		}

	}
	void RenderHandler::refreshActor(ECS::Actor actor) {
		UUID uuid = actor.getUUID();
		for (SurfaceMaterialClassOptionsFlags class_ : getRenderUsingClasses(uuid)) {
			materialRendering[class_][uuid].mesh = &actor.getComponent<Components::Mesh3DComponent>();
			materialRendering[class_][uuid].transform = &actor.getComponent<Components::TransformComponent>();
		}
	}

	SurfaceMaterialRenderInfo RenderHandler::buildRenderInfoFromActor(ECS::Actor actor, SurfaceMaterialClassOptionsFlags flags) {
		SHARD3D_ASSERT(actor.hasComponent<Components::Mesh3DComponent>() && "Actor must have a Mesh3DComponent if trying to be submitted to surface material rendering list!");
		
		SurfaceMaterialRenderInfo renderInfo{};
		renderInfo.mesh = &actor.getComponent<Components::Mesh3DComponent>();
		renderInfo.transform = &actor.getComponent<Components::TransformComponent>();

		for (int i = 0; i < renderInfo.mesh->materials.size(); i++) {
			if (ResourceHandler::retrieveSurfaceMaterial(renderInfo.mesh->materials[i])->getClass() != flags) continue;
			renderInfo.material_indices.push_back(i);
		}

		return SurfaceMaterialRenderInfo{renderInfo.transform, renderInfo.mesh, renderInfo.material_indices };
	}
	void RenderHandler::addToSurfaceMaterialRenderingList(UUID actorID, SurfaceMaterialClassOptionsFlags flags, const SurfaceMaterialRenderInfo& renderInfo) {
		if (renderInfo.material_indices.empty()) return;
		materialRendering[flags][actorID] = renderInfo;
	}
	void RenderHandler::rmvFromSurfaceMaterialRenderingList(UUID actorID, SurfaceMaterialClassOptionsFlags flags) {
		materialRendering[flags].erase(actorID);
	}
	void RenderHandler::switchSurfaceMaterialRenderingList(UUID actorID, SurfaceMaterialClassOptionsFlags old_flags, SurfaceMaterialClassOptionsFlags new_flags, const SurfaceMaterialRenderInfo& new_renderInfo) {
		auto renderInfo = materialRendering[old_flags].extract(actorID);
		materialRendering[new_flags].insert(std::move(renderInfo));
		// wtf is this?? past zilver can you remind me why this exists
	}

	std::vector<SurfaceMaterialClassOptionsFlags> RenderHandler::getRenderUsingClasses(UUID actorID) {
		std::vector<SurfaceMaterialClassOptionsFlags> vector;
		for (auto& [_class, val] : materialRendering) {
			if (val.contains(actorID)) vector.push_back(_class);
		}
		return std::vector<SurfaceMaterialClassOptionsFlags>(vector.begin(), vector.end());
	}
	void RenderHandler::clear() {
		for (auto& [flags, data] : materialRendering) 
			for (auto& [actor, data2] : data)
				data.erase(actor);
	}
	void RenderHandler::addMesh3DComponentToActor(ECS::Actor actor, AssetID mesh) {
		actor.addComponent<Components::Mesh3DComponent>(mesh);
		addActorToRenderList(actor);
	}
	void RenderHandler::rmvMesh3DComponentFromActor(ECS::Actor actor) {
		actor.killComponent<Components::Mesh3DComponent>();
		UUID uuid = actor.getUUID();
		for (SurfaceMaterialClassOptionsFlags class_ : getRenderUsingClasses(uuid)) {
			RenderHandler::rmvFromSurfaceMaterialRenderingList(uuid, class_);
		}
	}
}
