#pragma once

#include "material.h"
#include "../../s3dstd.h"
#include "texture.h"
#include "model.h"

#include "assetid.h"

namespace Shard3D {
	enum class AssetType {
		Unknown, Texture, Mesh3D, SurfaceMaterial, Level
	};
	class AssetUtils {
	public:
		static std::string truncatePath(const std::string& total);
		static AssetType discoverAssetType(const std::string& assetPath);
	};

	class AssetManager {
	public:
		static bool doesAssetExist(const std::string& assetPath);

		static void loadLevelAssets();

		static void importTexture(const std::string& sourcepath, const std::string& destpath, TextureLoadInfo info);
		static void importMesh(const std::string& sourcepath, const std::string& destpath, Mesh3DLoadInfo info);
		static void createMaterial(const std::string& destpath, rPtr<SurfaceMaterial> material);
		
		static void purgeAsset(const std::string& assetPath);

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

		static void init(EngineDevice& dvc);

		static void loadMesh(const AssetID& asset);
		static void unloadMesh(const AssetID& asset);
		static inline rPtr<Mesh3D>& retrieveMesh(const AssetID& asset) {
#ifndef ENSET_UNSAFE_ASSETS
			return retrieveMesh_safe(asset);
#else			
			return retrieveMesh_unsafe(asset);
#endif
		}

		static void loadTexture(const AssetID& asset);
		static void unloadTexture(const AssetID& asset);
		static inline rPtr<Texture2D>& retrieveTexture(const AssetID& asset) {
#ifndef ENSET_UNSAFE_ASSETS
			return retrieveTexture_safe(asset);
#else
			return retrieveTexture_unsafe(asset);
#endif
		}

		static void loadSurfaceMaterial(const AssetID& asset);
		static void loadSurfaceMaterialRecursive(const AssetID& asset);
		static void unloadSurfaceMaterial(const AssetID& asset);

		static void rebuildSurfaceMaterial(rPtr<SurfaceMaterial> material);
		static inline rPtr<SurfaceMaterial> retrieveSurfaceMaterial(const AssetID& asset) {
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
		static inline std::vector<AssetID> destroyTexQueue;
		static inline std::vector<AssetID> destroyMeshQueue; 
		static inline std::vector<AssetID> destroySurfaceMatQueue;
		static inline std::vector<rPtr<SurfaceMaterial>> rebuildSurfaceMaterialQueue;
		static void _buildSurfaceMaterial(rPtr<SurfaceMaterial> material);

		static rPtr<Texture2D>& retrieveTexture_unsafe			(const AssetID& asset);
		static rPtr<Texture2D>& retrieveTexture_safe			(const AssetID& asset);
		static rPtr<Mesh3D>& retrieveMesh_unsafe				(const AssetID& asset);
		static rPtr<Mesh3D>& retrieveMesh_safe					(const AssetID& asset);
		static rPtr<SurfaceMaterial>& retrieveSurfaceMaterial_safe	(const AssetID& asset);

		static inline hashMap<AssetKey, rPtr<Mesh3D>> meshAssets;
		static inline hashMap<AssetKey, rPtr<Texture2D>> textureAssets;
		static inline hashMap<AssetKey, rPtr<SurfaceMaterial>> surfaceMaterialAssets;

		static inline EngineDevice* engineDevice{};
		friend class _special_assets;
	};

	//class EditorHandler {
	//	public:	static inline std::string searchAssetOnLookupTable(AssetKey key) { return lookupTable[key]; }
	//	private: static inline std::unordered_map<AssetKey, std::string> lookupTable;
	//};

	class _special_assets {
	public:
		// engine only function, do not call this
		static void _editor_icons_load();
		// engine only function, do not call this
		static void _editor_icons_destroy();
		// engine only function, do not call this
		static inline hashMap<std::string, rPtr<Texture2D>> _editor_icons;

		static inline const char* _editor_icons_array[][2]{
			{"editor.play",						"assets/_engine/_editor/icon_play.png"			},
			{"editor.pause",					"assets/_engine/_editor/icon_pause.png"			},
			{"editor.stop",						"assets/_engine/_editor/icon_stop.png"			},
			{"editor.save",						"assets/_engine/_editor/icon_save.png"			},
			{"editor.load",						"assets/_engine/_editor/icon_load.png"			},
			{"editor.saveas",					"assets/_engine/_editor/icon_save_as.png"		},
			{"editor.pref",						"assets/_engine/_editor/icon_pref.png"			},
			{"editor.settings",					"assets/_engine/_editor/icon_gear.png"			},
			{"editor.preview",					"assets/_engine/_editor/preview.png"			},
			{"editor.layout",					"assets/_engine/_editor/icon_null"				},
			{"editor.viewport",					"assets/_engine/_editor/icon_monitor.png"		},
			{"editor.level",					"assets/_engine/_editor/icon_level.png"			},
			{"editor.launch",					"assets/_engine/_editor/launch_game.png"		},			
			{"editor.browser.folder",			"assets/_engine/_editor/icon_folder.png"		},
			{"editor.browser.file",				"assets/_engine/_editor/icon_file.png"			},
			{"editor.browser.file.tex",			"assets/_engine/_editor/icon_texture.png"		},
			{"editor.browser.file.msh",			"assets/_engine/_editor/icon_mesh.png"			},
			{"editor.browser.file.lvl",			"assets/_engine/_editor/icon_level_old.png"		},
			{"editor.browser.file.smt",			"assets/_engine/_editor/icon_material.png"		},
			{"editor.browser.file.cpp",			"assets/_engine/_editor/icon_file.png"			},
			{"editor.browser.file.csh",			"assets/_engine/_editor/icon_file.png"			},
			{"editor.browser.file.vba",			"assets/_engine/_editor/icon_file.png"			},
			{"editor.browser.file.0",			"assets/_engine/_editor/icon_file.png"			},
			{"component.light.point",			"assets/_engine/_editor/icon_lightpoint.png"	},
			{"component.light.spot",			"assets/_engine/_editor/icon_lightspot.png"		},
			{"component.light.directional",		"assets/_engine/_editor/icon_lightdir.png"		},
			{"component.audio",					"assets/_engine/_editor/icon_audio"				},
			{"component.camera",				"assets/_engine/_editor/icon_camera.png"		},
		};
	};
}