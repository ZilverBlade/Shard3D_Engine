#pragma once

#include "material.h"
#include "../../s3dstd.h"
#include "texture.h"
#include "model.h"
#include <vulkan/vulkan_core.h>

namespace Shard3D {
	class EngineApplication;
		class AssetManager {
		protected:
			enum LOD_Level {
				HighDetail = 0,
				MediumDetal = 1,
				LowDetail = 2,
				Invisible = -1
			};
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
			static void loadLevelAssets();

			//unlike other assets, textures and meshs should use a path as a key, 
			//since they directly load from there, rather than load into a wrapper/struct

			static hashMap<std::string, sPtr<EngineMesh>>& getMeshAssets() { return meshAssets; }
			static hashMap<std::string, sPtr<EngineTexture>>& getTextureAssets() { return textureAssets; }
				   
			static void emplaceMesh(const std::string& meshPath, MeshType meshType = MeshType::MESH_TYPE_OBJ);
			static inline sPtr<EngineMesh>& retrieveMesh(const std::string& path) {
#ifndef ENSET_CONFIDENT_ASSETS
				return retrieveMesh_NENSET_CONFIDENT_ASSETS(path);
#else			
				return retrieveMesh_ENSET_CONFIDENT_ASSETS(path);
#endif
			}

			static void emplaceTexture(const std::string& texturePath, int filter = VK_FILTER_LINEAR);
			static inline sPtr<EngineTexture>& retrieveTexture(const std::string& path) {
#ifndef ENSET_CONFIDENT_ASSETS
				return retrieveTexture_NENSET_CONFIDENT_ASSETS(path);
#else
				return retrieveTexture_ENSET_CONFIDENT_ASSETS(path);
#endif
			}

			static void emplaceMaterial(sPtr<SurfaceMaterial> material, const std::string& materialPath);
			static inline sPtr<SurfaceMaterial> retrieveMaterial(const std::string& path) {
#ifndef ENSET_CONFIDENT_ASSETS
				return retrieveSurfaceMaterial_NENSET_CONFIDENT_ASSETS(path);
#else
				return make_sPtr<SurfaceMaterial>();
#endif
			}

#pragma region Material shenanigans
			//static void loadMaterialsFromList(MaterialSystem::MaterialList& matlist);
			//static void emplaceMaterial(Material& material);
			//static Material retrieveMaterialByGUID(uint64_t guid);
			//static Material retrieveMaterialByPath(const std::string& path);
			//static hashMap<uint64_t, Material>& getMaterialAssets() { return materialAssets; }
			//static hashMap<uint64_t, MaterialSystem::MaterialList>& getMaterialListAssets() { return materialListAssets; }
#pragma endregion	

			static void setDevice(EngineDevice& dvc) { engineDevice = &dvc; }
		private:			
			static sPtr<EngineTexture>& retrieveTexture_ENSET_CONFIDENT_ASSETS(const std::string& path);
			static sPtr<EngineTexture>& retrieveTexture_NENSET_CONFIDENT_ASSETS(const std::string& path);
			static sPtr<EngineMesh>& retrieveMesh_ENSET_CONFIDENT_ASSETS(const std::string& path);
			static sPtr<EngineMesh>& retrieveMesh_NENSET_CONFIDENT_ASSETS(const std::string& path);
			static sPtr<SurfaceMaterial>& retrieveSurfaceMaterial_NENSET_CONFIDENT_ASSETS(const std::string& path);

			static inline hashMap<std::string, sPtr<EngineMesh>> meshAssets;
			static inline hashMap<std::string, sPtr<EngineTexture>> textureAssets;
			static inline hashMap<std::string, sPtr<SurfaceMaterial>> surfaceMaterialAssets;

			static inline EngineDevice* engineDevice{};

			static void clearAllAssetsAndDontAddDefaults();
			friend class Shard3D::EngineApplication;
			friend class _special_assets;
		};
	class _special_assets {
		// engine only function, do not call this
		static void _editor_icons_load();
		// engine only function, do not call this
		static void _editor_icons_destroy();
		// engine only function, do not call this
		static inline hashMap<std::string, sPtr<EngineTexture>> _editor_icons;

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
			{"editor.browser.file.tex",			"assets/_engine/tex/_editor/icon_file.png"			},
			{"editor.browser.file.msh",			"assets/_engine/tex/_editor/icon_file.png"			},
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

		friend class EngineApplication;
		friend class _EditorBillboardRenderer;
		friend class AssetExplorerPanel;
		friend class ImGuiLayer;
	};
}