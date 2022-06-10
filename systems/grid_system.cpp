#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "grid_system.hpp"
#include <stdexcept>
#include <array>
#include <map>

namespace Shard3D {

    GridSystem::GridSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{ device } {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    GridSystem::~GridSystem() {
        vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
    }

    void GridSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void GridSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        const char* vertFile = "grid.vert.spv";
        const char* fragFile = "grid.frag.spv";

        char* vertShader = (char*)(calloc(strlen(SHADER_FILES_PATH) + strlen(vertFile) - 1, 1));
        strncpy(vertShader, SHADER_FILES_PATH, strlen(SHADER_FILES_PATH));
        strncat(vertShader, vertFile, strlen(vertFile));

        char* fragShader = (char*)(calloc(strlen(SHADER_FILES_PATH) + strlen(fragFile) - 1, 1));
        strncpy(fragShader, SHADER_FILES_PATH, strlen(SHADER_FILES_PATH));
        strncat(fragShader, fragFile, strlen(fragFile));

        PipelineConfigInfo pipelineConfig{};
        EnginePipeline::defaultPipelineConfigInfo(pipelineConfig);
        EnginePipeline::enableAlphaBlending(pipelineConfig, VK_BLEND_OP_ADD);
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        enginePipeline = std::make_unique<EnginePipeline>(
            engineDevice,
            vertShader,
            fragShader,
            pipelineConfig
            );
    }

    void GridSystem::recreatePipeline(VkRenderPass renderPass) {
        enginePipeline->destroyGraphicsPipeline();
       vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);


//assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        EnginePipeline::defaultPipelineConfigInfo(pipelineConfig);
        EnginePipeline::enableAlphaBlending(pipelineConfig, VK_BLEND_OP_ADD);

        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;

        enginePipeline = std::make_unique<EnginePipeline>(
            engineDevice,
            "shaders/grid.vert.spv",
            "shaders/grid.frag.spv",
            pipelineConfig,
            true
       );
    }

    void GridSystem::render(FrameInfo& frameInfo) {
        enginePipeline->bind(frameInfo.commandBuffer);

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