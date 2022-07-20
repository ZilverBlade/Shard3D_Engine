#pragma once

#include "../engine_window.hpp"
#include "../pipeline.hpp"
#include "../device.hpp"
#include "../camera.hpp"
#include "../frame_info.hpp"
#include "../buffer.hpp"
#include "../renderer.hpp"
#include "../gui.hpp"
namespace Shard3D {
	class GUIRenderSystem {
	public:
		GUIRenderSystem();
		~GUIRenderSystem();

		GUIRenderSystem(const GUIRenderSystem&) = delete;
		GUIRenderSystem& operator=(const GUIRenderSystem&) = delete;

		void create(VkRenderPass renderPass);
		void destroy();

		void render(FrameInfo& frameInfo, GUI& gui);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		std::unique_ptr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
		std::unique_ptr<EngineDescriptorSetLayout> guiSystemLayout;
		std::unique_ptr<EngineDescriptorSetLayout> ssboLayout;
		VkDescriptorSet ssboDescriptorSet{};
		std::unique_ptr<EngineBuffer> pickBuffer;

		friend class GUILayer;
	};

}