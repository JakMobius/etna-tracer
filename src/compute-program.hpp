#pragma once

class ComputeProgram;

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <utility>
#include <etna/vk-wrappers/queue/vk-queue.hpp>
#include <etna/vk-wrappers/pipeline/vk-compute-pipeline-factory.hpp>
#include <etna/vk-wrappers/pipeline/vk-pipeline-shader-stage.hpp>
#include <etna/vk-wrappers/vk-shader.hpp>
#include <etna/vk-wrappers/descriptors.hpp>
#include <etna/vk-wrappers/command-buffer/vk-command-pool.hpp>
#include <etna/vk-wrappers/command-buffer/vk-command-buffer.hpp>
#include "etna/smart-buffer/smart-buffer-factory.hpp"
#include "compute-program-buffer.hpp"

struct ComputeProgramPushConstants {
    std::span<char> data;
};

class ComputeProgram {

    VK::UnownedInstance m_vk_instance;
    VK::PhysicalDevice m_physical_device;
    VK::Device m_device;
    VK::Queue m_compute_queue;
    VK::Queue m_transfer_queue;
    VK::DescriptorPool m_descriptor_pool;
    VK::DescriptorSetLayout m_descriptor_set_layout;
    std::unique_ptr<VK::DescriptorSetArray> m_descriptor_set_array;
    VK::PipelineLayout m_pipeline_layout;
    VK::Pipeline m_compute_pipeline;
    VK::CommandPool m_command_pool{};
    VK::CommandBuffer m_command_buffer{};
    VK::UnownedShaderModule m_shader_module;

    uint32_t m_compute_queue_index = 0;
    uint32_t m_transfer_queue_index = 0;

    std::vector<ComputeAppBuffer *> m_buffers{};
    ComputeProgramPushConstants m_push_constants;

public:
    ComputeProgram(VK::UnownedInstance vk_instance, VK::PhysicalDevice vk_device);

    void add_buffer(ComputeAppBuffer &buffer);

    void clean_pipeline();

    void create_pipeline();

    void initialize();

    void start_buffer();

    void end_buffer(const VK::Queue &queue);

    void upload_buffers();

    void download_buffers();

    void compute(int x, int y, int z);

    void set_shader(VK::UnownedShaderModule shader_module);

    const VK::PhysicalDevice& get_physical_device() const {
        return m_physical_device;
    }

    VK::PhysicalDevice& get_physical_device() {
        return m_physical_device;
    }

    const VK::Device& get_device() const {
        return m_device;
    }

    VK::Device& get_device() {
        return m_device;
    }

    VK::UnownedInstance get_instance() const {
        return m_vk_instance;
    }

    ComputeProgramPushConstants &get_push_constants();
};