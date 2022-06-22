#pragma once

#include "engine_window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "descriptor_pools.hpp"
#include "utils/engine_utils.hpp"
#include "wb3d/actor.hpp"
#include "wb3d/level.hpp"

#include "components.hpp"
#include <string>
#include "renderer.hpp"

#include "UI/LayerStack.hpp"

#include <memory>
#include <vector>
using namespace Shard3D::wb3d;

namespace Shard3D {
	class RunApp {
	public:
		static constexpr int WIDTH = 1280; //800
		static constexpr int HEIGHT = 720; //600
		const std::string WINDOW_NAME = "Shard3D Engine"; //engine name

		RunApp();
		~RunApp();


		RunApp(const RunApp&) = delete;
		RunApp& operator=(const RunApp&) = delete;

		void run();
	private:
		void setupDescriptors();
		void loadGameObjects();
		LayerStack layerStack;

		EngineWindow engineWindow { WIDTH, HEIGHT, WINDOW_NAME };//{ (int)getValFromGameConfig("WINDOW", "WIDTH"), (int)getValFromGameConfig("WINDOW", "HEIGHT"), WINDOW_NAME};
		EngineDevice engineDevice{ engineWindow };
		EngineRenderer engineRenderer{ engineWindow, engineDevice };

		std::shared_ptr<Level> activeLevel{};
};

}