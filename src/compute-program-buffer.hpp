#pragma once

class ComputeProgram;

#include "etna/smart-buffer/smart-buffer.hpp"

struct ComputeAppBuffer {
    int m_binding_id;
    bool m_should_upload = true;
    bool m_should_download = true;

    std::span<char> m_host_buffer{};
    std::unique_ptr<SmartBuffer> m_device_buffer{};
    ComputeProgram* m_program = nullptr;

    ComputeAppBuffer(int binding_id) : m_binding_id(binding_id), m_host_buffer({}) {

    }

    int get_binding() {
        return m_binding_id;
    }

    size_t get_byte_size() {
        return get_host_buffer().size_bytes();
    }

    void set_should_upload(bool should_upload) {
        m_should_upload = should_upload;
    }

    void set_should_download(bool should_download) {
        m_should_download = should_download;
    }

    bool get_should_upload() const {
        return m_should_upload;
    }

    bool get_should_download() const {
        return m_should_download;
    }

    std::span<char> &get_host_buffer() {
        return m_host_buffer;
    }

    SmartBuffer &get_device_buffer() {
        return *m_device_buffer.get();
    }

    void set_device_buffer(SmartBuffer buffer) {
        m_device_buffer = std::make_unique<SmartBuffer>(std::move(buffer));
    }

    void set_host_buffer(std::span<char> buffer);

    void set_program(ComputeProgram *program) {
        m_program = program;
    }
};