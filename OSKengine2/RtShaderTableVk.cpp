#include "RtShaderTableVk.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVk.h"
#include "GpuMemoryTypes.h"
#include "IGpuMemoryAllocator.h"
#include "RendererVk.h"
#include "GpuMemoryBlockVk.h"
#include "IGpuMemorySubblock.h"

#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

RtShaderTableVk::RtShaderTableVk(TSize numShaderGroups, VkPipeline pipeline) {
	const auto& info = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetInfo();

	const TSize handleSize = info.rtPipelineProperties.shaderGroupHandleSize;
	const TSize handleSizeWithAlignment = (info.rtPipelineProperties.shaderGroupHandleSize + info.rtPipelineProperties.shaderGroupHandleAlignment - 1) & ~(info.rtPipelineProperties.shaderGroupHandleAlignment - 1);
	const TSize numGroups = numShaderGroups;

	const TSize tableSize = numGroups * handleSizeWithAlignment;

	DynamicArray<TByte> temporalStorage = DynamicArray<TByte>::CreateResizedArray(tableSize);

	// Obtiene los handles para los grupos y los almacena en temporalStorage.
	VkResult result = RendererVk::pvkGetRayTracingShaderGroupHandlesKHR(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(), pipeline,
		0, numGroups, tableSize, temporalStorage.GetData());
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear la tabla de shaders RT. Code: " + std::to_string(result));

	const TSize alignment = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetInfo().rtPipelineProperties.shaderGroupHandleAlignment;
	raygenShaderTable = Engine::GetRenderer()->GetAllocator()->CreateBuffer(handleSize, alignment, GpuBufferUsage::RT_SHADER_BINDING_TABLE, GpuSharedMemoryType::GPU_AND_CPU).GetPointer();
	hitShaderTable = Engine::GetRenderer()->GetAllocator()->CreateBuffer(handleSize, alignment, GpuBufferUsage::RT_SHADER_BINDING_TABLE, GpuSharedMemoryType::GPU_AND_CPU).GetPointer();
	missShaderTable = Engine::GetRenderer()->GetAllocator()->CreateBuffer(handleSize, alignment, GpuBufferUsage::RT_SHADER_BINDING_TABLE, GpuSharedMemoryType::GPU_AND_CPU).GetPointer();

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
