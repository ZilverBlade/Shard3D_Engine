#pragma once

#include "../engine_window.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"
		  
#include "../wb3d/scene.hpp"
#include "../wb3d/actor.hpp"

#include "../components.hpp"
#include <string>
#include "../renderer.hpp"

#include <memory>
#include <vector>

namespace Shard3D {
	class DirectionalLightSystem {
	public:
		DirectionalLightSystem(EngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~DirectionalLightSystem();

		DirectionalLightSystem(const DirectionalLightSystem&) = delete;
		DirectionalLightSystem& operator=(const DirectionalLightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo &ubo, std::shared_ptr<wb3d::Scene>& scene);
		void render(FrameInfo &frameInfo, std::shared_ptr<wb3d::Scene>& scene);

	private:

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		EngineDevice& engineDevice;

		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
	};

}