#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "compute_system.hpp"
#include <stdexcept>
#include <array>
#include <map>

namespace Shard3D {

    ComputeSystem::ComputeSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
        createPipeline(renderPass, globalSetLayout);
    }

    ComputeSystem::~ComputeSystem() {
        vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
    }

    void ComputeSystem::createPipeline(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) {
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
 
        if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        EnginePipeline::defaultPipelineConfigInfo(pipelineConfig);

        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;


        enginePipeline = std::make_unique<EnginePipeline>(
            "had to add this because std::make_unique gets the wrong overload",
            engineDevice,
            VK_SHADER_STAGE_COMPUTE_BIT,
            pipelineLayout,
            "assets/shaders/test.comp.spv",
            pipelineConfig
        );
    }


    void ComputeSystem::render(FrameInfo& frameInfo) {
        enginePipeline->bindCompute(frameInfo.commandBuffer);      
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr
        );
        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
}