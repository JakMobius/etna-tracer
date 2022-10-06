#pragma once

#include <etna/vk-wrappers/device.hpp>
#include <etna/vk-wrappers/buffer/vk-buffer-factory.hpp>
#include "smart-buffer.hpp"

class SmartBufferFactory {
    VK::BufferFactory m_factory {};
    SmartBufferType m_type = SmartBufferType::with_staging_buffer;

public:
    SmartBufferFactory& set_mode(SmartBufferType type) {
        m_type = type;
        return *this;
    }

    SmartBufferFactory& set_size(VkDeviceSize size) {
        m_factory.set_size(size);
        return *this;
    }

    SmartBufferFactory& set_usage(VkBufferUsageFlags usage) {
        m_factory.set_usage(usage);
        return *this;
    }

    SmartBufferFactory& set_flags(VkBufferCreateFlags flags) {
        m_factory.set_flags(flags);
        return *this;
    }

    std::vector<uint32_t>& get_queue_families() {
        return m_factory.get_queue_families();
    }

    SmartBuffer create(VK::Device* device) {
        switch(m_type) {
            case with_staging_buffer:
                break;
            case without_staging_buffer:
                m_factory.set_memory_properties(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
                break;
        }

        auto result = SmartBuffer(m_factory.create_memory_buffer(device));
        switch(m_type) {
            case with_staging_buffer:
                result.set_use_own_staging_buffer(true);
                break;
            case without_staging_buffer:
                result.set_use_own_staging_buffer(false);
                break;
        }
        return result;
    }
};