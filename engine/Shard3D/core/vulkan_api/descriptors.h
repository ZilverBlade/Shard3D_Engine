#pragma once
#include "device.h"
#include "../rendering/swap_chain.h"

namespace Shard3D {

    class EngineDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(EngineDevice& engineDevice) : engineDevice{ engineDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            uPtr<EngineDescriptorSetLayout> build() const;

        private:
            EngineDevice& engineDevice;
            hashMap<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        EngineDescriptorSetLayout(
            EngineDevice& engineDevice, hashMap<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~EngineDescriptorSetLayout();
        EngineDescriptorSetLayout(const EngineDescriptorSetLayout&) = delete;
        EngineDescriptorSetLayout& operator=(const EngineDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        EngineDevice& engineDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        hashMap<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class EngineDescriptorWriter;
    };

    class EngineDescriptorPool {
    public:
        class Builder {
        public:
            Builder(EngineDevice& engineDevice) : engineDevice{ engineDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            uPtr<EngineDescriptorPool> build() const;

        private:
            EngineDevice& engineDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        EngineDescriptorPool(
            EngineDevice& engineDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~EngineDescriptorPool();
        EngineDescriptorPool(const EngineDescriptorPool&) = delete;
        EngineDescriptorPool& operator=(const EngineDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(const std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        EngineDevice& engineDevice;
        VkDescriptorPool descriptorPool;

        friend class EngineDescriptorWriter;
    };

    class EngineDescriptorWriter {
    public:
        EngineDescriptorWriter(EngineDescriptorSetLayout& setLayout, EngineDescriptorPool& pool);

        EngineDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        EngineDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        EngineDescriptorSetLayout& setLayout;
        EngineDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

    class SharedPools {
    public:
        // (0 = global, 1 = per-material static, 2 = per-material dynamic, 3 = per-draw), 
        // https://www.reddit.com/r/gamedev/comments/kybh22/vulkan_material_handler_descriptor_management_and/

        static inline uPtr<EngineDescriptorPool> globalPool{};
        static inline uPtr<EngineDescriptorPool> staticMaterialPool{};
        static inline uPtr<EngineDescriptorPool> dynamicMaterialPool{};
        static inline std::vector<uPtr<EngineDescriptorPool>> drawPools;

        static inline void constructPools(EngineDevice& device) {
            // Global pool
            globalPool = EngineDescriptorPool::Builder(device)
                .setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();

            staticMaterialPool = EngineDescriptorPool::Builder(device)
                .setMaxSets(1024)
                .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 512)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 512)
                .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 256)
                .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
                .build();

            // Draw pools
            drawPools.resize(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
            auto framePoolBuilder = EngineDescriptorPool::Builder(device)
                .setMaxSets(1024)
                .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1024)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1024)
                .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024)
                .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
            for (int i = 0; i < drawPools.size(); i++) {
                drawPools[i] = framePoolBuilder.build();
            }
        }

        static inline void destructPools() {
            globalPool = nullptr;
            staticMaterialPool = nullptr;
            dynamicMaterialPool = nullptr;
            for (int i = 0; i < drawPools.size(); i++) {
                drawPools[i] = nullptr;
            }
        }
    };
} 