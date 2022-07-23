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
		inline static constexpr int WIDTH = 1280; //800
		inline static constexpr int HEIGHT = 720; //600
		inline static const std::string WINDOW_NAME = "Shard3D Engine"; //engine name
	public:
		Singleton() {}
		~Singleton() {}
		inline static Initializer _ignore_init;

		inline static EngineWindow engineWindow{ WIDTH, HEIGHT, WINDOW_NAME };
		inline static EngineDevice engineDevice{ engineWindow };
		inline static EngineRenderer engineRenderer{ engineWindow, engineDevice };
		inline static VkDescriptorSet viewportImage{};
		inline static OffScreen mainOffScreen{ engineDevice };

#if ALLOW_PREVIEW_CAMERA
		inline static OffScreen previewCamOffScreen{ engineDevice };
		inline static VkDescriptorSet previewViewportImage{};
#endif
		inline static std::shared_ptr<wb3d::Level> activeLevel, capturedLevel;

		inline static ImGui_ImplVulkan_InitInfo imgui_init_info{};

//#ifndef _DEPLOY
		struct eps {
			bool V_EDITOR_BILLBOARDS = true;
			bool V_HIDDEN_MESHES = true;
			bool V_HIDDEN_BILLBOARDS = true;
			bool V_GRID = true;
			bool V_GUI = false;
			bool ONLY_GAME;
		};
		inline static eps editorPreviewSettings;
//#endif
		inline static glm::vec3 testPBR = {1.f, 0.5f, 0.f};

		inline static Destructor _ignore_destroy;
	};
}