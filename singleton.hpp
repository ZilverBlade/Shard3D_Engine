 #pragma once

#include "device.hpp"
#include "UI/imgui_implementation.hpp"
#include "renderer.hpp"
#include "wb3d/level.hpp"
#include "offscreen.hpp"
#include "graphics_settings.hpp"
#include "audio.hpp"
#include "scripts/dynamic_script_engine.hpp"
namespace Shard3D {
	// ignore
	class Initializer {
	private:
		friend class Singleton;
		Initializer() {
			LOGGER::init();
			EngineAudio::init();
			GraphicsSettings::init(nullptr);
			DynamicScriptEngine::init();
		}
	};
	class Destructor {
	private:
		friend class Singleton;
		Destructor() {}
		~Destructor();
	};
	class Singleton {
	private:
		_S3D_GVAR constexpr int WIDTH = 1280; //800
		_S3D_GVAR constexpr int HEIGHT = 720; //600
		_S3D_GVAR const std::string WINDOW_NAME = "Shard3D Engine"; //engine name
	public:
		Singleton() {}
		~Singleton() {}
		_S3D_GVAR Initializer _ignore_init;

		_S3D_GVAR EngineWindow engineWindow{ WIDTH, HEIGHT, WINDOW_NAME };
		_S3D_GVAR EngineDevice engineDevice{ engineWindow };
		_S3D_GVAR EngineRenderer engineRenderer{ engineWindow, engineDevice };
		_S3D_GVAR VkDescriptorSet viewportImage{};
		_S3D_GVAR OffScreen mainOffScreen{ engineDevice };

#if ALLOW_PREVIEW_CAMERA
		_S3D_GVAR OffScreen previewCamOffScreen{ engineDevice };
		_S3D_GVAR VkDescriptorSet previewViewportImage{};
#endif
		_S3D_GVAR std::shared_ptr<wb3d::Level> activeLevel, capturedLevel;

		_S3D_GVAR ImGui_ImplVulkan_InitInfo imgui_init_info{};

//#ifndef _DEPLOY
		struct eps {
			bool V_EDITOR_BILLBOARDS = true;
			bool V_HIDDEN_MESHES = true;
			bool V_HIDDEN_BILLBOARDS = true;
			bool V_GRID = true;
			bool V_GUI = false;
			bool ONLY_GAME;
		};
		_S3D_GVAR eps editorPreviewSettings;
//#endif
		_S3D_GVAR glm::vec3 testPBR = {1.f, 0.5f, 0.f};

		_S3D_GVAR Destructor _ignore_destroy;
	};
}