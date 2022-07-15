#include "../s3dtpch.h"
#include <glm/gtc/constants.hpp>

#include "../singleton.hpp"
#include "compute_system.hpp"
#include "../texture.hpp" 

namespace Shard3D {

    ComputeSystem::ComputeSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
        createPipeline(renderPass, globalSetLayout);
    }

    ComputeSystem::~ComputeSystem() {
        vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
    }

    void ComputeSystem::createPipeline(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) {
        //pipeline layout
        computeSystemLayout =
            EngineDescriptorSetLayout::Builder(engineDevice)
            .addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
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
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        EnginePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;

        enginePipeline = std::make_unique<EnginePipeline>(
            "had to add this because std::make_unique gets the wrong overload",
            engineDevice,
            VK_SHADER_STAGE_COMPUTE_BIT,
            pipelineLayout,
            "assets/shaderdata/test.comp.spv",
            pipelineConfig
        );

        srcImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }


    void ComputeSystem::render(FrameInfo& frameInfo) {
        enginePipeline->bindCompute(frameInfo.commandBuffer);

        srcImageInfo.imageView = Singleton::mainOffScreen.getImageView();
        srcImageInfo.sampler = Singleton::mainOffScreen.getSampler();

        VkDescriptorImageInfo computeImageInfo;

        computeImageInfo.sampler = Singleton::mainOffScreen.getSampler();
        computeImageInfo.imageView = Singleton::mainOffScreen.getImageView();
        computeImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        auto computeSetLayout = EngineDescriptorSetLayout::Builder(engineDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
            .build();

        VkDescriptorSet descriptorSet1;
        EngineDescriptorWriter(*computeSystemLayout, frameInfo.perDrawDescriptorPool)
            .writeImage(1, &srcImageInfo)
            //.writeImage(2, &dstImageInfo)
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