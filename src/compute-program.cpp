
#include "compute-program.hpp"

#include <utility>

ComputeProgram::ComputeProgram(VK::UnownedInstance vk_instance,
                               VK::PhysicalDevice vk_device) : m_vk_instance(std::move(vk_instance)),
                                                                 m_physical_device(std::move(vk_device)) {}

void ComputeProgram::add_buffer(ComputeAppBuffer &buffer) {
    buffer.set_program(this);
    m_buffers.push_back(&buffer);
}

void ComputeProgram::update_buffer(ComputeAppBuffer& buffer) {
    set_needs_pipeline_rebuild();

    SmartBufferFactory scene_buffer_factory;

    scene_buffer_factory.set_size(buffer.get_host_buffer().size() * sizeof(buffer.get_host_buffer()[0]));
    scene_buffer_factory.set_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                                   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    scene_buffer_factory.get_queue_families().push_back(m_compute_queue_index);
    scene_buffer_factory.get_queue_families().push_back(m_transfer_queue_index);

    buffer.set_device_buffer(scene_buffer_factory.create(&m_device));
}

void ComputeProgram::clean_pipeline() {
    m_compute_pipeline.destroy();
    m_pipeline_layout.destroy();
    m_descriptor_set_layout.destroy();
    m_descriptor_pool.destroy();
}

void ComputeProgram::create_pipeline() {

    m_update_pipeline = false;

    VK::ComputePipelineFactory pipeline_factory{};

    m_descriptor_pool = VK::DescriptorPoolFactory()
            .add_pool_size(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2)
            .set_flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
            .set_max_sets(2)
            .create(&m_device);

    VK::DescriptorSetLayoutFactory descriptor_set_layout_factory;

    for (auto &buffer: m_buffers) {
        if(!buffer->m_device_buffer) continue;

        VK::DescriptorSetLayoutBinding binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

        binding.set_descriptor_count(1).set_stage_flags(VK_SHADER_STAGE_COMPUTE_BIT);
        descriptor_set_layout_factory.bind_descriptor(buffer->get_binding(), binding);
    }

    m_descriptor_set_layout = descriptor_set_layout_factory.create(&m_device);

    m_descriptor_set_array = std::make_unique<VK::DescriptorSetArray>(m_descriptor_pool);
    m_descriptor_set_array->get_layouts().resize(1, m_descriptor_set_layout.get_handle());
    m_descriptor_set_array->create();

    for (auto buffer: m_buffers) {
        if(!buffer->m_device_buffer) continue;

        VK::StorageBufferDescriptor buffer_descriptor(buffer->get_device_buffer().get_buffer(), 0,
                                                      buffer->get_byte_size());

        m_descriptor_set_array->bind_descriptor(0, buffer->get_binding(), buffer_descriptor);
    }

    VkDescriptorSetLayout descriptors[]{m_descriptor_set_layout.get_handle()};

    VkPushConstantRange push_constant;
    push_constant.offset = 0;
    push_constant.size = m_push_constants.data.size_bytes();
    push_constant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkPushConstantRange push_constants[]{push_constant};

    m_pipeline_layout = VK::PipelineLayout::create(&m_device, descriptors, push_constants);

    pipeline_factory.m_shader_stage.set_shader(m_shader_module, VK_SHADER_STAGE_COMPUTE_BIT);

    m_compute_pipeline = pipeline_factory.create(m_pipeline_layout);
}

void ComputeProgram::update_pipeline_if_needed() {
    if (m_update_pipeline) {
        clean_pipeline();
        create_pipeline();
    }
}

void ComputeProgram::initialize() {
    m_compute_queue_index = m_physical_device.get_queue_family_indices()->find_family(VK_QUEUE_COMPUTE_BIT);
    m_transfer_queue_index = m_physical_device.get_queue_family_indices()->find_family(VK_QUEUE_TRANSFER_BIT);

    float queue_priority = 1;

    auto device_factory = VK::DeviceFactory();
    device_factory.add_queue(0, m_compute_queue_index, {&queue_priority, 1});
    if (m_transfer_queue_index != m_compute_queue_index) {
        device_factory.add_queue(0, m_transfer_queue_index, {&queue_priority, 1});
    }

    device_factory.get_validation_layers().push_back("VK_LAYER_KHRONOS_validation");
    m_device = device_factory.create(&m_physical_device);

    m_transfer_queue = m_device.get_queue(m_transfer_queue_index, 0);
    m_compute_queue = m_device.get_queue(m_compute_queue_index, 0);

    m_command_pool = VK::CommandPool::create(
            &m_device,
            m_compute_queue_index,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
    );
}

void ComputeProgram::start_buffer() {
    if (m_command_buffer) {
        m_command_buffer.reset();
        m_command_buffer.begin(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    } else {
        m_command_buffer = m_command_pool.create_command_buffer();
        m_command_buffer.begin(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    }
}

void ComputeProgram::end_buffer(const VK::Queue &queue) {
    VK::Fence fence = VK::Fence::create(&m_device);

    m_command_buffer.end();
    m_command_buffer.submit(queue, fence);

    fence.wait_one();
}

void ComputeProgram::upload_buffers() {

    bool need_transfer = false;
    for (auto &buffer: m_buffers) {
        if (buffer->get_should_upload()) {
            need_transfer = true;
            break;
        }
    }

    if (!need_transfer) return;

    start_buffer();

    for (auto &buffer: m_buffers) {
        if (buffer->get_should_upload() && buffer->m_device_buffer) {
            buffer->get_device_buffer().update_data(m_command_buffer, 0, buffer->get_host_buffer());
            buffer->set_should_upload(false);
        }
    }

    end_buffer(m_transfer_queue);
}

void ComputeProgram::download_buffers() {
    start_buffer();

    for (auto &buffer: m_buffers) {
        if (buffer->get_should_download()) {
            buffer->get_device_buffer().write_download_command_if_needed(m_command_buffer, 0,
                                                                         buffer->get_byte_size());
        }
    }

    end_buffer(m_transfer_queue);

    for (auto &buffer: m_buffers) {
        if (buffer->get_should_download()) {
            buffer->get_device_buffer().read_downloaded_data(0, buffer->get_host_buffer());
            buffer->set_should_download(false);
        }
    }
}

void ComputeProgram::compute(int x, int y, int z) {

    update_pipeline_if_needed();
    upload_buffers();

    start_buffer();
    m_command_buffer.bind_pipeline(VK_PIPELINE_BIND_POINT_COMPUTE, m_compute_pipeline);

    VkDescriptorSet descriptors[] = {m_descriptor_set_array->get_descriptor_sets()[0]};
    m_command_buffer.bind_descriptor_sets(VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline_layout.get_handle(),
                                          descriptors, {});

    vkCmdPushConstants(m_command_buffer.get_handle(), m_pipeline_layout.get_handle(), VK_SHADER_STAGE_COMPUTE_BIT,
                       0, m_push_constants.data.size_bytes(), m_push_constants.data.data());
    vkCmdDispatch(m_command_buffer.get_handle(), x, y, z);

    end_buffer(m_compute_queue);
}

ComputeProgramPushConstants &ComputeProgram::get_push_constants() {
    return m_push_constants;
}

void ComputeProgram::set_shader(VK::UnownedShaderModule shader_module) {
    m_shader_module = std::move(shader_module);
}

void ComputeProgram::set_needs_pipeline_rebuild() {
    m_update_pipeline = true;
}
