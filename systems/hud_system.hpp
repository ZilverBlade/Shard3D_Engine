#pragma once

#include "../engine_window.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"
#include "../buffer.hpp"
#include "../renderer.hpp"
#include "../hud.hpp"
namespace Shard3D {
	class HUDRenderSystem {
	public:
		HUDRenderSystem();
		~HUDRenderSystem();

		void reset();

		HUDRenderSystem(const HUDRenderSystem&) = delete;
		HUDRenderSystem& operator=(const HUDRenderSystem&) = delete;

		void create(VkRenderPass renderPass);
		void destroy();
		uint64_t getSelectedID();
		void render(FrameInfo& frameInfo, HUD& gui);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
		std::unique_ptr<EngineDescriptorSetLayout> guiSystemLayout;
		std::unique_ptr<EngineDescriptorSetLayout> ssboLayout;
		VkDescriptorSet ssboDescriptorSet{};
		std::unique_ptr<EngineBuffer> pickBuffer;

		friend class HUDLayer;
	};

}