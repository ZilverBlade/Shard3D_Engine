#pragma once
#include "s3dtpch.h"
#include "device.hpp"
#include "UI/imgui_implementation.hpp"
#include "renderer.hpp"
#include "wb3d/level.hpp"
#include "offscreen.hpp"
#include "graphics_settings.hpp"

namespace Shard3D {
	// ignore
	class Initializer {
	private:
		friend class Singleton;
		Initializer() {
			LOGGER::init();
			GraphicsSettings::init(nullptr);
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
		inline static OffScreen mainOffScreen{ engineDevice };
		inline static VkDescriptorSet viewportImage{};
#if ALLOW_PREVIEW_CAMERA
		inline static OffScreen previewCamOffScreen{ engineDevice };
		inline static VkDescriptorSet previewViewportImage{};
#endif
		inline static std::shared_ptr<wb3d::Level> activeLevel{};

		inline static ImGui_ImplVulkan_InitInfo imgui_init_info{};

		Destructor _ignore_destroy;
	};
}