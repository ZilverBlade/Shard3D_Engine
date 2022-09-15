#pragma once

#include "material.h"
#include "../../s3dstd.h"
#include "texture.h"
#include "model.h"

#include "assetid.h"

namespace std {
	template<>
	struct hash<Shard3D::AssetID> {
		size_t operator()(const Shard3D::AssetID& id) const {
			return id.getID();
		}
	};
}

namespace Shard3D {
	enum class AssetType {
		Unknown, Texture, Model3D, SurfaceMaterial, PostProcessingMaterial, Level
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
		static void importMesh(const std::string& sourcepath, const std::string& destpath, Model3DLoadInfo info);
		static void createMaterial(const std::string& destpath, rPtr<SurfaceMaterial> material);
		static void createMaterial(const std::string& destpath, rPtr<PostProcessingMaterial> material);

		static void purgeAsset(const std::string& assetPath);

		static void setDevice(EngineDevice& dvc) { engineDevice = &dvc; }
	private:
		static inline EngineDevice* engineDevice{};

		friend class _special_assets;
	};
	class ResourceHandler {
	public:							
		struct __CoreAssets {
			const AssetID t_errorTexture =			AssetID("assets/_engine/tex/null_tex.png.s3dasset");		// Purple checkerboard texture
			const AssetID t_errorMaterialTexture =	AssetID("assets/_engine/tex/null_mat.png.s3dasset");		// Blank checkerboard texture
			const AssetID t_whiteTexture =			AssetID("assets/_engine/tex/0x000000.png.s3dasset");		// Black texture
			const AssetID t_blackTexture =			AssetID("assets/_engine/tex/0xffffff.png.s3dasset");		// White texture
			const AssetID t_normalTexture =			AssetID("assets/_engine/tex/0x807ffe.png.s3dasset");		// Blank normal texture;
			 
			const AssetID m_errorMesh =				AssetID("assets/_engine/msh/null_mdl.obj.s3dasset");		// No Mesh model
			const AssetID m_defaultModel =			AssetID("assets/_engine/msh/cube.obj.s3dasset");			// Cube Mesh model
			 
			const AssetID s_errorMaterial =			AssetID("assets/_engine/mat/world_grid.s3dasset");		// Opaque surface material (world grid)
			const AssetID s_blankMaterial = AssetID("assets/_engine/mat/world_grid_blank.s3dasset");		// Opaque surface material (world grid)
		} static inline coreAssets;

		//Clears textureAssets
		static void clearTextureAssets();
		//Clears meshAssets
		static void clearMeshAssets();
		//Clears materialAssets
		static void clearMaterialAssets();
		//Clears all of the asset maps
		static void clearAllAssets();
		static void clearAllUnusedAssets();
		/* Loads all of the materials in use by the level into the asset maps.
		Make sure to clear before loading, since you dont want to waste resources pointing to unused assets!
		*/

		// Destroys all assets and doesnt keep the core engine ones either
		static void destroy();

		static void init(EngineDevice& dvc);

		static void loadMesh(const AssetID& asset);
		static void unloadMesh(const AssetID& asset);
		static inline rPtr<Model3D>& retrieveMesh(const AssetID& asset) {
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
			return retrieveSurfaceMaterial_unsafe(asset);
#endif
		}

		static void loadPPOMaterial(const AssetID& asset);
		static void unloadPPOMaterial(const AssetID& asset);

		static void rebuildPPOMaterial(rPtr<PostProcessingMaterial> material);
		static inline rPtr<PostProcessingMaterial> retrievePPOMaterial(const AssetID& asset) {
#ifndef ENSET_UNSAFE_ASSETS
			return retrievePPOMaterial_safe(asset);
#else
			return retrievePPOMaterial_unsafe(asset);
#endif
		}

		static auto& getMeshAssets() { return meshAssets; }
		static auto& getTextureAssets() { return textureAssets; }
		static auto& getSurfaceMaterialAssets() { return surfaceMaterialAssets; }
		static auto& getPPOMaterialAssets() { return ppoMaterialAssets; }
		static void runGarbageCollector();

	private:
		static inline std::vector<AssetID> destroyTexQueue;
		static inline std::vector<AssetID> destroyMeshQueue; 
		static inline std::vector<AssetID> destroySurfaceMatQueue;
		static inline std::vector<AssetID> destroyPPOMatQueue;
		static inline std::vector<rPtr<SurfaceMaterial>> rebuildSurfaceMaterialQueue;
		static inline std::vector<rPtr<PostProcessingMaterial>> rebuildPPOMaterialQueue;
		static void _buildSurfaceMaterial(rPtr<SurfaceMaterial> material);
		static void _buildPPOMaterial(rPtr<PostProcessingMaterial> material);

		static rPtr<Texture2D>& retrieveTexture_unsafe			(const AssetID& asset);
		static rPtr<Texture2D>& retrieveTexture_safe			(const AssetID& asset);
		static rPtr<Model3D>& retrieveMesh_unsafe				(const AssetID& asset);
		static rPtr<Model3D>& retrieveMesh_safe					(const AssetID& asset);
		static rPtr<SurfaceMaterial>& retrieveSurfaceMaterial_safe	(const AssetID& asset);
		static rPtr<SurfaceMaterial>& retrieveSurfaceMaterial_unsafe(const AssetID& asset);
		static rPtr<PostProcessingMaterial>& retrievePPOMaterial_safe(const AssetID& asset);
		static rPtr<PostProcessingMaterial>& retrievePPOMaterial_unsafe(const AssetID& asset);

		static inline hashMap<AssetID, rPtr<Model3D>> meshAssets;
		static inline hashMap<AssetID, rPtr<Texture2D>> textureAssets;
		static inline hashMap<AssetID, rPtr<SurfaceMaterial>> surfaceMaterialAssets;
		static inline hashMap<AssetID, rPtr<PostProcessingMaterial>> ppoMaterialAssets;
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
			{"editor.layout",					"assets/_engine/_editor/icon_layout.png"		},
			{"editor.viewport",					"assets/_engine/_editor/icon_monitor.png"		},
			{"editor.level",					"assets/_engine/_editor/icon_level.png"			},
			{"editor.launch",					"assets/_engine/_editor/launch_game.png"		},			
			{"editor.browser.navback",			"assets/_engine/_editor/icon_back.png"			},
			{"editor.browser.navfwd",			"assets/_engine/_editor/icon_forward.png"		},
			{"editor.browser.refresh",			"assets/_engine/_editor/icon_refresh.png"		},
			{"editor.browser.folder",			"assets/_engine/_editor/icon_folder.png"		},
			{"editor.browser.file",				"assets/_engine/_editor/icon_whatfile.png"		},
			{"editor.browser.file.tex",			"assets/_engine/_editor/icon_texture.png"		},
			{"editor.browser.file.msh",			"assets/_engine/_editor/icon_mesh.png"			},
			{"editor.browser.file.lvl",			"assets/_engine/_editor/icon_level_old.png"		},
			{"editor.browser.file.smt",			"assets/_engine/_editor/icon_material.png"		},
			{"editor.browser.file.pmt",			"assets/_engine/_editor/icon_whatfile.png"		},
			{"editor.browser.file.0",			"assets/_engine/_editor/icon_whatfile.png"		},
			{"component.light.point",			"assets/_engine/_editor/icon_lightpoint.png"	},
			{"component.light.spot",			"assets/_engine/_editor/icon_lightspot.png"		},
			{"component.light.directional",		"assets/_engine/_editor/icon_lightdir.png"		},
			{"component.audio",					"assets/_engine/_editor/icon_audio.png"			},
			{"component.camera",				"assets/_engine/_editor/icon_camera.png"		},
		};
	};
}