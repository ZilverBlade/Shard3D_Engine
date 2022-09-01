#include "../../s3dpch.h" 

#include <glm/gtc/constants.hpp>

#include "compute_system.h"
#include "../../core/asset/texture.h" 

namespace Shard3D {

    ComputeSystem::ComputeSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
        createPipeline(renderPass, globalSetLayout);
    }

    ComputeSystem::~ComputeSystem() {
        vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
    }

    void ComputeSystem::createPipeline(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) {
        std::shared_ptr<Texture2D> outputTargetImg = Texture2D::createEmptyTexture(engineDevice);
        //pipeline layout
        computeSystemLayout =
            EngineDescriptorSetLayout::Builder(engineDevice)
            .addBinding(6, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // input
            .addBinding(7, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // output target
            .build();

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
            globalSetLayout,
            computeSystemLayout->getDescriptorSetLayout() 
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

        srcImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        
        dstImageInfo.imageView = outputTargetImg->getImageView();
        dstImageInfo.sampler = outputTargetImg->getSampler();
        srcImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    }


    void ComputeSystem::render(FrameInfo& frameInfo) {

        computePipeline->bindCompute(frameInfo.commandBuffer);

        //srcImageInfo.imageView = Singleton::mainOffScreen.getImageView();
        //srcImageInfo.sampler = Singleton::mainOffScreen.getSampler();

        VkDescriptorImageInfo computeImageInfo;

       //computeImageInfo.sampler = Singleton::mainOffScreen.getSampler();
       //computeImageInfo.imageView = Singleton::mainOffScreen.getImageView();
       //computeImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        EngineDescriptorWriter(*computeSystemLayout, frameInfo.perDrawDescriptorPool)
            .writeImage(6, &srcImageInfo)
            .writeImage(7, &dstImageInfo)
            .build(descriptorSet1);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            pipelineLayout,
            1,  // first set
            1,  // set count
            &descriptorSet1,
            0,
            nullptr);
    }

	
}