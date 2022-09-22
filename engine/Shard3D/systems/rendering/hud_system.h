#pragma once
#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  

#include "../../core/ui/hud.h"

namespace Shard3D {
	inline namespace Systems {
		class HUDRenderSystem {
		public:
			HUDRenderSystem();
			~HUDRenderSystem();

			void reset();

			HUDRenderSystem(const HUDRenderSystem&) = delete;
			HUDRenderSystem& operator=(const HUDRenderSystem&) = delete;

			void create(EngineDevice& dvc, EngineWindow& wnd, VkRenderPass renderPass);
			void destroy();
			uint64_t getSelectedID();
			void render(FrameInfo& frameInfo, HUD& gui);
		private:
			void createPipelineLayout();
			void createPipeline(VkRenderPass renderPass);

			uPtr<GraphicsPipeline> graphicsPipeline;
			uPtr<EngineDescriptorSetLayout> guiSystemLayout;
			uPtr<EngineDescriptorSetLayout> ssboLayout;

			uPtr<EngineBuffer> pickBuffer;

			VkPipelineLayout pipelineLayout;
			VkDescriptorSet ssboDescriptorSet{};

			EngineDevice* device;
			EngineWindow* window;

			friend class HUDLayer;
		};
	}
}