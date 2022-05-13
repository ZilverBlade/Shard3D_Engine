#pragma once

#include "shard_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace shard {

    class ShardDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(ShardDevice& shardDevice) : shardDevice{ shardDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<ShardDescriptorSetLayout> build() const;

        private:
            ShardDevice& shardDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        ShardDescriptorSetLayout(
            ShardDevice& shardDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~ShardDescriptorSetLayout();
        ShardDescriptorSetLayout(const ShardDescriptorSetLayout&) = delete;
        ShardDescriptorSetLayout& operator=(const ShardDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        ShardDevice& shardDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class ShardDescriptorWriter;
    };

    class ShardDescriptorPool {
    public:
        class Builder {
        public:
            Builder(ShardDevice& shardDevice) : shardDevice{ shardDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<ShardDescriptorPool> build() const;

        private:
            ShardDevice& shardDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        ShardDescriptorPool(
            ShardDevice& shardDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~ShardDescriptorPool();
        ShardDescriptorPool(const ShardDescriptorPool&) = delete;
        ShardDescriptorPool& operator=(const ShardDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        ShardDevice& shardDevice;
        VkDescriptorPool descriptorPool;

        friend class ShardDescriptorWriter;
    };

    class ShardDescriptorWriter {
    public:
        ShardDescriptorWriter(ShardDescriptorSetLayout& setLayout, ShardDescriptorPool& pool);

        ShardDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        ShardDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        ShardDescriptorSetLayout& setLayout;
        ShardDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace shard