#include "../../s3dpch.h" 

#include <glm/gtc/constants.hpp>

#include "compute_system.h"
#include "../../core/asset/texture.h" 

namespace Shard3D {

    ComputeSystem::ComputeSystem(EngineDevice& device) : engineDevice{ device } {
        createPipeline();
    }

    ComputeSystem::~ComputeSystem() {
        vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
    }

    void ComputeSystem::createPipeline() {
        //std::shared_ptr<Texture2D> outputTargetImg = Texture2D::createEmptyTexture(engineDevice);
        //pipeline layout
       //computeSystemLayout =
       //    EngineDescriptorSetLayout::Builder(engineDevice)
       //    .addBinding(6, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // input
       //    .addBinding(7, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // output target
       //    .build();

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
        };
        
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
 
        if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            SHARD3D_FATAL("failed to create pipeline layout!");
        }

        //pipeline
        SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        computePipeline = make_uPtr<ComputePipeline>(
            engineDevice,
            pipelineLayout,
            "assets/shaderdata/test.comp.spv"
        );

        //srcImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        //
        //dstImageInfo.imageView = outputTargetImg->getImageView();
        //dstImageInfo.sampler = outputTargetImg->getSampler();
        //srcImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    }


    void ComputeSystem::execute(FrameInfo& frameInfo) {

        computePipeline->bindCompute(frameInfo.commandBuffer);

        vkCmdDispatch(frameInfo.commandBuffer, 1, 1, 1);
    }

	
}