#include "../../../s3dpch.h" 
#include <glm/gtc/constants.hpp>

#include "grid_system.h"

namespace Shard3D {

    GridSystem::GridSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ &device } {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    GridSystem::~GridSystem() {
        vkDestroyPipelineLayout(engineDevice->device(), pipelineLayout, nullptr);
    }

    void GridSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(engineDevice->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            SHARD3D_FATAL("failed to create pipeline layout!");
        }
    }

    void GridSystem::createPipeline(VkRenderPass renderPass) {
        SHARD3D_ASSERT(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        GraphicsPipelineConfigInfo pipelineConfig{};
        GraphicsPipeline::pipelineConfig(pipelineConfig)
            .defaultGraphicsPipelineConfigInfo()
            .enableAlphaBlending(VK_BLEND_OP_ADD)
            .setSubpass(1);

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        graphicsPipeline = make_uPtr<GraphicsPipeline>(
            *engineDevice,
            "assets/shaderdata/_editor/grid.vert.spv",
            "assets/shaderdata/_editor/grid.frag.spv",
            pipelineConfig
            );
    }

    void GridSystem::render(FrameInfo& frameInfo) {
        graphicsPipeline->bind(frameInfo.commandBuffer);
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
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