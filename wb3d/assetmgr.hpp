#pragma once

#include "../systems/material_system.hpp"
#include "level.hpp"
#include "../texture.hpp"
#include "../model.hpp"

namespace Shard3D {
	class EditorApp;
	namespace wb3d {
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
			//Clears all of the asset maps
			static void clearAllAssets();
			/* Loads all of the materials in use by the level into the asset maps.
			Make sure to clear before loading, since you dont want to waste resources pointing to unused assets!
			*/
			static void loadLevelAssets();

			//unlike other assets, textures and meshs should use a path as a key, 
			//since they directly load from there, rather than load into a wrapper/struct

			static std::unordered_map<std::string, std::shared_ptr<EngineMesh>>& getMeshAssets() { return meshAssets; }
			static std::unordered_map<std::string, std::shared_ptr<EngineTexture>>& getTextureAssets() { return textureAssets; }

			static void emplaceMesh(const std::string& meshPath, MeshType meshType = MeshType::MESH_TYPE_OBJ);
			static std::shared_ptr<EngineMesh>& retrieveMesh(const std::string& path);

			static void emplaceTexture(const std::string& texturePath, VkFilter filter = VK_FILTER_LINEAR);
			static std::shared_ptr<EngineTexture>& retrieveTexture(const std::string& path);

#pragma region Material shenanigans
			static void loadMaterialsFromList(MaterialSystem::MaterialList& matlist);
			static void emplaceMaterial(MaterialSystem::Material& material);
			static MaterialSystem::Material retrieveMaterialByGUID(uint64_t guid);
			static MaterialSystem::Material retrieveMaterialByPath(const std::string& path);
			static std::unordered_map<uint64_t, MaterialSystem::Material>& getMaterialAssets() { return materialAssets; }
			static std::unordered_map<uint64_t, MaterialSystem::MaterialList>& getMaterialListAssets() { return materialListAssets; }
#pragma endregion	
		private:
			_S3D_GVAR std::unordered_map<std::string, std::shared_ptr<EngineMesh>> meshAssets;
			_S3D_GVAR std::unordered_map<std::string, std::shared_ptr<EngineTexture>> textureAssets;
			_S3D_GVAR std::unordered_map<uint64_t, MaterialSystem::Material> materialAssets;
			_S3D_GVAR std::unordered_map<uint64_t, MaterialSystem::MaterialList> materialListAssets;

			static void clearAllAssetsAndDontAddDefaults();
			friend class Shard3D::EditorApp;
		};
	}
	class _special_assets {
		// engine only function, do not call this
		static void _editor_icons_load();
		// engine only function, do not call this
		static void _editor_icons_destroy();
		// engine only function, do not call this
		_S3D_GVAR std::unordered_map<std::string, std::shared_ptr<EngineTexture>> _editor_icons;

		_S3D_GVAR const char* _editor_icons_array[][2]{
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

		friend class EditorApp;
		friend class _EditorBillboardRenderer;
		friend class AssetExplorerPanel;
		friend class ImGuiLayer;
	};
}