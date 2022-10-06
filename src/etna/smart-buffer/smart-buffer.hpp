#pragma once

#include <etna/vk-wrappers/buffer.hpp>
#include <etna/vk-wrappers/command-buffer/vk-command-buffer.hpp>
#include <etna/vk-wrappers/commands/vk-copy-buffer-command.hpp>

enum SmartBufferType {
    with_staging_buffer,
    without_staging_buffer
};

class SmartBuffer {
    friend class SmartBufferFactory;

    VK::MemoryBuffer m_memory_buffer {{}};
    std::unique_ptr<VK::MemoryBuffer> m_staging_buffer {};
    bool m_use_own_staging_buffer = true;

    void create_staging_buffer() {
        auto device = m_memory_buffer.get_memory().get_device();
        VK::BufferFactory factory;

        // TODO: separate src and dst
        factory.set_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
        factory.set_memory_properties(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        factory.set_size(m_memory_buffer.get_memory().get_size());
        m_staging_buffer = std::make_unique<VK::MemoryBuffer>(factory.create_memory_buffer(device));
    }

    explicit SmartBuffer(VK::MemoryBuffer&& buffer): m_memory_buffer(std::move(buffer)) {}

public:
    SmartBuffer(SmartBuffer&& move) = default;
    SmartBuffer& operator=(SmartBuffer&& move_assign) = default;
    ~SmartBuffer() { destroy(); }

    template<typename T>
    void update_data(const VK::UnownedCommandBuffer& command_buffer, VkDeviceSize offset, const T& data) {
        VkDeviceSize size = data.size() * sizeof(data[0]);
        if(!m_use_own_staging_buffer) {
            update_data_directly(offset, size, data.data());
            return;
        }
        if(!m_staging_buffer) create_staging_buffer();
        update_data_through_staging_buffer(command_buffer, offset, size, data.data(), m_staging_buffer.get());
    }

    void update_data_through_staging_buffer(const VK::UnownedCommandBuffer& command_buffer,
                                            VkDeviceSize offset,
                                            VkDeviceSize size,
                                            const void* data,
                                            VK::MemoryBuffer* staging_buffer) {
        staging_buffer->get_memory().set_data(data, size, offset);

        VK::CopyBufferCommand(&staging_buffer->get_buffer(), &m_memory_buffer.get_buffer())
                .set_src_offset(0)
                .set_dst_offset(offset)
                .set_size(size)
                .write(command_buffer);
    }

    void update_data_directly(VkDeviceSize offset,
                              VkDeviceSize size,
                              const void* data) {

        m_memory_buffer.get_memory().set_data(data, size, offset);
    }

    template<typename T>
    void read_downloaded_data(VkDeviceSize offset, T& data) {
        VkDeviceSize size = data.size() * sizeof(data[0]);
        if(!m_use_own_staging_buffer) {
            read_data_directly(offset, size, data.data());
        } else {
            m_staging_buffer->get_memory().get_data(data.data(), size, offset);
        }
    }

    void write_download_command_if_needed(const VK::UnownedCommandBuffer& command_buffer,
                                          VkDeviceSize offset,
                                          VkDeviceSize size,
                                          VK::MemoryBuffer* staging_buffer = nullptr) {
        if(!staging_buffer) {
            staging_buffer = m_staging_buffer.get();

            if(!staging_buffer) {
                if(m_use_own_staging_buffer) {
                    create_staging_buffer();
                    staging_buffer = m_staging_buffer.get();
                } else {
                    return;
                }
            }
        }

        VK::CopyBufferCommand(&m_memory_buffer.get_buffer(), &staging_buffer->get_buffer())
                .set_src_offset(offset)
                .set_dst_offset(0)
                .set_size(size)
                .write(command_buffer);
    }

    void read_data_directly(VkDeviceSize offset,
                              VkDeviceSize size,
                              void* data) {

        m_memory_buffer.get_memory().get_data(data, size, offset);
    }

    void delete_own_staging_buffer() {
        m_staging_buffer = nullptr;
    }

    bool has_own_staging_buffer() {
        return m_staging_buffer != nullptr;
    }

    void set_use_own_staging_buffer(bool use_staging_buffer) {
        if(!use_staging_buffer) delete_own_staging_buffer();
        m_use_own_staging_buffer = use_staging_buffer;
    }

    void destroy() {
        m_memory_buffer.destroy();
        delete_own_staging_buffer();
    }

    VK::Buffer& get_buffer() { return m_memory_buffer.get_buffer(); }
    const VK::Buffer& get_buffer() const { return m_memory_buffer.get_buffer(); }

    VK::Memory& get_memory() { return m_memory_buffer.get_memory(); }
    const VK::Memory& get_memory() const { return m_memory_buffer.get_memory(); }
};