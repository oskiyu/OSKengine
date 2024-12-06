#include "RtShaderTableVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVk.h"
#include "GpuMemoryTypes.h"
#include "IGpuMemoryAllocator.h"
#include "RendererVk.h"
#include "GpuMemoryBlockVk.h"
#include "IGpuMemorySubblock.h"

#include <vulkan/vulkan.h>
#include "AccelerationStructuresExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

RtShaderTableVk::RtShaderTableVk(USize32 numShaderGroups, VkPipeline pipeline) {
	const auto& info = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetInfo();

	const USize64 handleSize = info.rtPipelineProperties.shaderGroupHandleSize;
	const USize64 handleSizeWithAlignment = (info.rtPipelineProperties.shaderGroupHandleSize + info.rtPipelineProperties.shaderGroupHandleAlignment - 1) & ~(info.rtPipelineProperties.shaderGroupHandleAlignment - 1);
	const USize32 numGroups = numShaderGroups;

	const USize64 tableSize = numGroups * handleSizeWithAlignment;

	DynamicArray<TByte> temporalStorage = DynamicArray<TByte>::CreateResized(tableSize);

	// Obtiene los handles para los grupos y los almacena en temporalStorage.
	VkResult result = RendererVk::pvkGetRayTracingShaderGroupHandlesKHR(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(), pipeline,
		0, numGroups, tableSize, temporalStorage.GetData());
	OSK_ASSERT(result == VK_SUCCESS, RtShaderBindingTableCreationException(result));

	const USize64 alignment = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetInfo().rtPipelineProperties.shaderGroupHandleAlignment;
	raygenShaderTable = Engine::GetRenderer()->GetAllocator()->CreateBuffer(handleSize, alignment, GpuBufferUsage::RT_SHADER_BINDING_TABLE, GpuSharedMemoryType::GPU_AND_CPU, GpuQueueType::MAIN).GetPointer();
	hitShaderTable = Engine::GetRenderer()->GetAllocator()->CreateBuffer(handleSize, alignment, GpuBufferUsage::RT_SHADER_BINDING_TABLE, GpuSharedMemoryType::GPU_AND_CPU, GpuQueueType::MAIN).GetPointer();
	missShaderTable = Engine::GetRenderer()->GetAllocator()->CreateBuffer(handleSize, alignment, GpuBufferUsage::RT_SHADER_BINDING_TABLE, GpuSharedMemoryType::GPU_AND_CPU, GpuQueueType::MAIN).GetPointer();

	raygenShaderTable->MapMemory();
	raygenShaderTable->Write(temporalStorage.GetData() + handleSizeWithAlignment * 0, handleSize);
	raygenShaderTable->Unmap();

	hitShaderTable->MapMemory();
	hitShaderTable->Write(temporalStorage.GetData() + handleSizeWithAlignment * 2, handleSize);
	hitShaderTable->Unmap();

	missShaderTable->MapMemory();
	missShaderTable->Write(temporalStorage.GetData() + handleSizeWithAlignment * 1, handleSize);
	missShaderTable->Unmap();

	// Para obtener el address.
	VkBufferDeviceAddressInfoKHR bufferAddressInfo{};
	bufferAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;

	bufferAddressInfo.buffer = raygenShaderTable->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer();
	raygenTableAddressRegion.deviceAddress = RendererVk::pvkGetBufferDeviceAddressKHR(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(), &bufferAddressInfo) + raygenShaderTable->GetMemorySubblock()->GetOffsetFromBlock();
	raygenTableAddressRegion.size = handleSizeWithAlignment;
	raygenTableAddressRegion.stride = handleSizeWithAlignment * 1; // * num entries

	bufferAddressInfo.buffer = hitShaderTable->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer();
	hitTableAddressRegion.deviceAddress = RendererVk::pvkGetBufferDeviceAddressKHR(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(), &bufferAddressInfo) + hitShaderTable->GetMemorySubblock()->GetOffsetFromBlock();
	hitTableAddressRegion.size = handleSizeWithAlignment;
	hitTableAddressRegion.stride = handleSizeWithAlignment * 1; // * num entries

	bufferAddressInfo.buffer = missShaderTable->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer();
	missTableAddressRegion.deviceAddress = RendererVk::pvkGetBufferDeviceAddressKHR(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(), &bufferAddressInfo) + missShaderTable->GetMemorySubblock()->GetOffsetFromBlock();
	missTableAddressRegion.size = handleSizeWithAlignment;
	missTableAddressRegion.stride = handleSizeWithAlignment * 1; // * num entries
}

VkStridedDeviceAddressRegionKHR RtShaderTableVk::GetRaygenTableAddressRegion() const {
	return raygenTableAddressRegion;
}

VkStridedDeviceAddressRegionKHR RtShaderTableVk::GetClosestHitTableAddressRegion() const {
	return hitTableAddressRegion;
}

VkStridedDeviceAddressRegionKHR RtShaderTableVk::GetMissTableAddressRegion() const {
	return missTableAddressRegion;
}

#endif
