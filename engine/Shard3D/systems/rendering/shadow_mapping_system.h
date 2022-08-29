#pragma once
#include "../../s3dstd.h"
#include "../../vulkan_abstr.h"
#include "../../core/misc/frame_info.h"	  
#include "../../core/rendering/renderpass.h"

namespace Shard3D {
	class ShadowMappingSystem {
	public:
		ShadowMappingSystem(EngineDevice& device);
		~ShadowMappingSystem();

		ShadowMappingSystem(const ShadowMappingSystem&) = delete;
		ShadowMappingSystem& operator=(const ShadowMappingSystem&) = delete;

		void render(FrameInfo& frameInfo);
		FrameBufferAttachment* getFrameBufferAttachment() {
			return shadowDepthFramebufferAttachment;
		}
	private:
		void createRenderPass();
		void createPipelineLayout();
		void createPipeline();
		
		EngineCamera lightCamera{};
		glm::mat4 lightProjection;
		EngineDevice& engineDevice;

		FrameBufferAttachment* shadowDepthFramebufferAttachment;
		FrameBuffer* shadowFrameBuffer;
		SimpleRenderPass* shadowRenderpass;

		uPtr<EnginePipeline> enginePipeline;
		VkPipelineLayout pipelineLayout;
		uPtr<EngineDescriptorSetLayout> billboardSystemLayout;
	};
}