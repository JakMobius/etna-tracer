
#include <vector>
#include "compute-program-buffer.hpp"
#include "compute-program.hpp"

void ComputeAppBuffer::set_host_buffer(std::span<char> buffer) {
    m_host_buffer = buffer;
    if(m_program) {
        m_program->update_buffer(*this);
    }
}
