#pragma once

#include "material.h"
#include "../../s3dstd.h"
#include "texture.h"
#include "model.h"

#include "../misc/assetid.h"

namespace Shard3D {
	enum class AssetType {
		Unknown, Texture, Mesh3D, Material, Level
	};
	class AssetUtils {
	public:
		static std::string truncatePath(const std::string& total);
		static AssetType discoverAssetType(const std::string& assetPath);
	};

	class AssetManager {
	public:
		static void loadLevelAssets();

		static void importTexture(const std::string& sourcepath, const std::string& destpath, TextureLoadInfo info);
		static void importMesh(const std::string& sourcepath, const std::string& destpath, MeshLoadInfo info);
		static void createMaterial(const std::string& destpath, rPtr<SurfaceMaterial> material);
		
		static void setDevice(EngineDevice& dvc) { engineDevice = &dvc; }
	private:
		static inline EngineDevice* engineDevice{};

		friend class _special_assets;
	};
	class ResourceHandler {
	public:
		//Clears textureAssets
		static void clearTextureAssets();
		//Clears meshAssets
		static void clearMeshAssets();
		//Clears materialAssets
		static void clearMaterialAssets();
		//Clears all of the asset maps
		static void clearAllAssets();
		/* Loads all of the materials in use by the level into the asset maps.
		Make sure to clear before loading, since you dont want to waste resources pointing to unused assets!
		*/

		// Destroys all assets and doesnt keep the core engine ones either
		static void destroy();

		static void setDevice(EngineDevice& dvc) { engineDevice = &dvc; }

		static void loadMesh(const AssetID& asset);
		static inline rPtr<EngineMesh>& retrieveMesh(const AssetID& asset) {
#ifndef ENSET_UNSAFE_ASSETS
			return retrieveMesh_safe(asset);
#else			
			return retrieveMesh_unsafe(asset);
#endif
		}

		static void loadTexture(const AssetID& assetPath);
		static inline rPtr<EngineTexture>& retrieveTexture(const AssetID& asset) {
#ifndef ENSET_UNSAFE_ASSETS
			return retrieveTexture_safe(asset);
#else
			return retrieveTexture_unsafe(asset);
#endif
		}

		static void loadMaterial(rPtr<SurfaceMaterial> material, const std::string& materialPath);
		static inline rPtr<SurfaceMaterial> retrieveMaterial(const AssetID& asset) {
#ifndef ENSET_UNSAFE_ASSETS
			return retrieveSurfaceMaterial_safe(asset);
#else
			return make_rPtr<SurfaceMaterial>();
#endif
		}

		static auto& getMeshAssets() { return meshAssets; }
		static auto& getTextureAssets() { return textureAssets; }
		static auto& getSurfaceMaterialAssets() { return surfaceMaterialAssets; }

		static void runGarbageCollector();
	private:
		static inline std::vector<AssetID> actorReloadTexQueue;
		static inline std::vector<AssetID> reloadMeshQueue;
		static void _loadMesh(const AssetID& asset);

		static rPtr<EngineTexture>& retrieveTexture_unsafe			(const AssetID& asset);
		static rPtr<EngineTexture>& retrieveTexture_safe			(const AssetID& asset);
		static rPtr<EngineMesh>& retrieveMesh_unsafe				(const AssetID& asset);
		static rPtr<EngineMesh>& retrieveMesh_safe					(const AssetID& asset);
		static rPtr<SurfaceMaterial>& retrieveSurfaceMaterial_safe	(const AssetID& asset);

		static inline hashMap<AssetKey, rPtr<EngineMesh>> meshAssets;
		static inline hashMap<AssetKey, rPtr<EngineTexture>> textureAssets;
		static inline hashMap<AssetKey, rPtr<SurfaceMaterial>> surfaceMaterialAssets;

		static inline EngineDevice* engineDevice{};
		friend class _special_assets;
	};
	class _special_assets {
	public:
		// engine only function, do not call this
		static void _editor_icons_load();
		// engine only function, do not call this
		static void _editor_icons_destroy();
		// engine only function, do not call this
		static inline hashMap<std::string, rPtr<EngineTexture>> _editor_icons;

		static inline const char* _editor_icons_array[][2]{
			{"editor.play",						"assets/_engine/tex/_editor/icon_play.png"			},
			{"editor.pause",					"assets/_engine/tex/_editor/icon_pause.png"			},
			{"editor.stop",						"assets/_engine/tex/_editor/icon_stop.png"			},
			{"editor.save",						"assets/_engine/tex/_editor/icon_save.png"			},
			{"editor.load",						"assets/_engine/tex/_editor/icon_load.png"			},
			{"editor.pref",						"assets/_engine/tex/_editor/icon_pref.png"			},
			{"editor.settings",					"assets/_engine/tex/_editor/icon_gear.png"			},
			{"editor.preview",					"assets/_engine/tex/_editor/preview.png"			},
			{"editor.layout",					"assets/_engine/tex/_editor/icon_null"				},
			{"editor.viewport",					"assets/_engine/tex/_editor/icon_monitor.png"		},
			{"editor.level",					"assets/_engine/tex/_editor/icon_level.png"			},
			{"editor.browser.folder",			"assets/_engine/tex/_editor/icon_folder.png"		},
			{"editor.browser.file",				"assets/_engine/tex/_editor/icon_file.png"			},
			{"editor.browser.file.tex",			"assets/_engine/tex/_editor/icon_texture.png"		},
			{"editor.browser.file.msh",			"assets/_engine/tex/_editor/icon_mesh.png"			},
			{"editor.browser.file.lvl",			"assets/_engine/tex/_editor/icon_level_old.png"		},
			{"editor.browser.file.cpp",			"assets/_engine/tex/_editor/icon_file.png"			},
			{"editor.browser.file.csh",			"assets/_engine/tex/_editor/icon_file.png"			},
			{"editor.browser.file.vba",			"assets/_engine/tex/_editor/icon_file.png"			},
			{"editor.browser.file.0",			"assets/_engine/tex/_editor/icon_file.png"			},
			{"component.light.point",			"assets/_engine/tex/_editor/icon_lightpoint.png"	},
			{"component.light.spot",			"assets/_engine/tex/_editor/icon_lightspot.png"		},
			{"component.light.directional",		"assets/_engine/tex/_editor/icon_lightdir.png"		},
			{"component.audio",					"assets/_engine/tex/_editor/icon_null"				},
			{"component.camera",				"assets/_engine/tex/_editor/icon_null"				},
		};
	};
}