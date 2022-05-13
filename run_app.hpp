#pragma once

#include "shard_window.hpp"
#include "shard_pipeline.hpp"
#include "shard_device.hpp"
#include "shard_descriptors.hpp"

#include "shard_game_object.hpp"
#include <string>
#include "shard_renderer.hpp"

#include <memory>
#include <vector>

namespace shard {
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
		void loadGameObjects();

		ShardWindow shardWindow{ WIDTH, HEIGHT, WINDOW_NAME };
		ShardDevice shardDevice{ shardWindow };
		ShardRenderer shardRenderer{ shardWindow, shardDevice };

		// note: order of declaration matters
		std::unique_ptr<ShardDescriptorPool> globalPool{};
		ShardGameObject::Map gameObjects;
};

}