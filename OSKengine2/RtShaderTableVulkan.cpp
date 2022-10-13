#include "RtShaderTableVulkan.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVulkan.h"
#include "GpuMemoryTypes.h"
#include "IGpuMemoryAllocator.h"
#include "RendererVulkan.h"
#include "GpuMemoryBlockVulkan.h"
#include "IGpuMemorySubblock.h"

#include <vulkan/vulkan.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

RtShaderTableVulkan::RtShaderTableVulkan(TSize numShaderGroups, VkPipeline pipeline) {
	const auto& info = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetInfo();

	const TSize handleSize = info.rtPipelineProperties.shaderGroupHandleSize;
	const TSize handleSizeWithAlignment = (info.rtPipelineProperties.shaderGroupHandleSize + info.rtPipelineProperties.shaderGroupHandleAlignment - 1) & ~(info.rtPipelineProperties.shaderGroupHandleAlignment - 1);
	const TSize numGroups = numShaderGroups;

	const TSize tableSize = numGroups * handleSizeWithAlignment;

	DynamicArray<TByte> temporalStorage = DynamicArray<TByte>::CreateResizedArray(tableSize);

	// Obtiene los handles para los grupos y los almacena en temporalStorage.
	VkResult result = RendererVulkan::pvkGetRayTracingShaderGroupHandlesKHR(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(), pipeline,
		0, numGroups, tableSize, temporalStorage.GetData());
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear la tabla de shaders RT. Code: " + std::to_string(result));

	const TSize alignment = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetInfo().rtPipelineProperties.shaderGroupHandleAlignment;
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

	bufferAddressInfo.buffer = raygenShaderTable->GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer();
	raygenTableAddressRegion.deviceAddress = RendererVulkan::pvkGetBufferDeviceAddressKHR(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(), &bufferAddressInfo) + raygenShaderTable->GetMemorySubblock()->GetOffsetFromBlock();
	raygenTableAddressRegion.size = handleSizeWithAlignment;
	raygenTableAddressRegion.stride = handleSizeWithAlignment * 1; // * num entries

	bufferAddressInfo.buffer = hitShaderTable->GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer();
	hitTableAddressRegion.deviceAddress = RendererVulkan::pvkGetBufferDeviceAddressKHR(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(), &bufferAddressInfo) + hitShaderTable->GetMemorySubblock()->GetOffsetFromBlock();
	hitTableAddressRegion.size = handleSizeWithAlignment;
	hitTableAddressRegion.stride = handleSizeWithAlignment * 1; // * num entries

	bufferAddressInfo.buffer = missShaderTable->GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer();
	missTableAddressRegion.deviceAddress = RendererVulkan::pvkGetBufferDeviceAddressKHR(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(), &bufferAddressInfo) + missShaderTable->GetMemorySubblock()->GetOffsetFromBlock();
	missTableAddressRegion.size = handleSizeWithAlignment;
	missTableAddressRegion.stride = handleSizeWithAlignment * 1; // * num entries
}

VkStridedDeviceAddressRegionKHR RtShaderTableVulkan::GetRaygenTableAddressRegion() const {
	return raygenTableAddressRegion;
}

VkStridedDeviceAddressRegionKHR RtShaderTableVulkan::GetClosestHitTableAddressRegion() const {
	return hitTableAddressRegion;
}

VkStridedDeviceAddressRegionKHR RtShaderTableVulkan::GetMissTableAddressRegion() const {
	return missTableAddressRegion;
}
