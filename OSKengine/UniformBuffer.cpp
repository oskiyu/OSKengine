#include "UniformBuffer.h"

using namespace OSK;

const std::vector<SharedPtr<GpuDataBuffer>>& UniformBuffer::GetBuffers() const {
	return buffers;
}

std::vector<SharedPtr<GpuDataBuffer>>& UniformBuffer::GetBuffersRef() {
	return buffers;
}

size_t UniformBuffer::GetBufferSize() const {
	return buffers[0]->GetSize();
}

uint32_t UniformBuffer::GetBufferAlignment() const {
	return buffers[0]->GetAlignment();
}
