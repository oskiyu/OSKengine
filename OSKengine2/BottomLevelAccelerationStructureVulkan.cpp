#include "BottomLevelAccelerationStructureVulkan.h"

#include "GpuVertexBufferVulkan.h"
#include "GpuIndexBufferVulkan.h"
#include "OSKengine.h"
#include "RendererVulkan.h"
#include "IGpuMemoryAllocator.h"
#include "GpuMemoryTypes.h"
#include "GpuMemoryBlockVulkan.h"
#include "IGpuMemorySubblock.h"
#include "GpuVulkan.h"
#include "CommandListVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void BottomLevelAccelerationStructureVulkan::Setup(const IGpuVertexBuffer& vertexBuffer, const IGpuIndexBuffer& indexBuffer) {
	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
	IGpuMemoryAllocator* memoryAllocator = Engine::GetRenderer()->GetMemoryAllocator();

	matrixBuffer = Engine::GetRenderer()->GetMemoryAllocator()->CreateBuffer(sizeof(VkTransformMatrixKHR), GpuBufferUsage::RT_ACCELERATION_STRUCTURE_BUILDING, GpuSharedMemoryType::GPU_AND_CPU).GetPointer();
	SetMatrix(glm::mat4(1.0f));

	// Obtene la localización en la memoria de la GPU de los buffers.
	VkBufferDeviceAddressInfoKHR bufferAddressInfo{};
	bufferAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;

	VkDeviceOrHostAddressConstKHR vertexBufferGpuAddress{};
	bufferAddressInfo.buffer = vertexBuffer.GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer();
	vertexBufferGpuAddress.deviceAddress = RendererVulkan::pvkGetBufferDeviceAddressKHR(logicalDevice, &bufferAddressInfo);

	VkDeviceOrHostAddressConstKHR indexBufferGpuAddress{};
	bufferAddressInfo.buffer = indexBuffer.GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer();
	indexBufferGpuAddress.deviceAddress = RendererVulkan::pvkGetBufferDeviceAddressKHR(logicalDevice, &bufferAddressInfo);
	
	VkDeviceOrHostAddressConstKHR matrixBufferGpuAddress{};
	bufferAddressInfo.buffer = matrixBuffer->GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer();
	matrixBufferGpuAddress.deviceAddress = RendererVulkan::pvkGetBufferDeviceAddressKHR(logicalDevice, &bufferAddressInfo);

	// Tamaño del vértice
	TSize vertexStride = 0;
	for (const auto& i : vertexBuffer.GetVertexInfo().entries)
		vertexStride += i.size;

	// Info de la geometría
	geometryInfo = VkAccelerationStructureGeometryKHR{};

	geometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	geometryInfo.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	geometryInfo.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	geometryInfo.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	geometryInfo.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
	geometryInfo.geometry.triangles.vertexData = vertexBufferGpuAddress;
	geometryInfo.geometry.triangles.maxVertex = vertexBuffer.GetNumVertices();
	geometryInfo.geometry.triangles.vertexStride = vertexStride;
	geometryInfo.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
	geometryInfo.geometry.triangles.indexData = indexBufferGpuAddress;
	geometryInfo.geometry.triangles.transformData.deviceAddress = 0;
	geometryInfo.geometry.triangles.transformData.hostAddress = nullptr;
	geometryInfo.geometry.triangles.transformData = matrixBufferGpuAddress;

	numTriangles = indexBuffer.GetNumTriangles();

	// Rango único para toda la geometría
	VkAccelerationStructureBuildRangeInfoKHR blasBuildRangeInfo{};
	blasBuildRangeInfo.primitiveCount = numTriangles;
	blasBuildRangeInfo.primitiveOffset = 0;
	blasBuildRangeInfo.firstVertex = 0;
	blasBuildRangeInfo.transformOffset = 0;

	// Obtenemos la cantidad de memoria necesaria
	VkAccelerationStructureBuildGeometryInfoKHR blasBuildGeometryInfo{};
	blasBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	blasBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	blasBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	blasBuildGeometryInfo.geometryCount = 1;
	blasBuildGeometryInfo.pGeometries = &geometryInfo;

	VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
	sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	RendererVulkan::pvkGetAccelerationStructureBuildSizesKHR(
		logicalDevice,
		VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
		&blasBuildGeometryInfo,
		&numTriangles,
		&sizeInfo);

	accelerationStructureBuffer = memoryAllocator->CreateBuffer(sizeInfo.accelerationStructureSize, GpuBufferUsage::RT_ACCELERATION_STRUCTURE, GpuSharedMemoryType::GPU_ONLY).GetPointer();
	UniquePtr<GpuDataBuffer> buildBuffer = memoryAllocator->CreateBuffer(sizeInfo.buildScratchSize, GpuBufferUsage::RT_ACCELERATION_STRUCTURE_BUILDING, GpuSharedMemoryType::GPU_AND_CPU).GetPointer();

	// Creación
	VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo{};
	accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	accelerationStructureCreateInfo.buffer = accelerationStructureBuffer->GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer();
	accelerationStructureCreateInfo.size = sizeInfo.accelerationStructureSize;
	accelerationStructureCreateInfo.offset = accelerationStructureBuffer->GetMemorySubblock()->GetOffsetFromBlock();
	accelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	
	VkResult result = RendererVulkan::pvkCreateAccelerationStructureKHR(logicalDevice, &accelerationStructureCreateInfo, nullptr, &accelerationStructureHandle);
	OSK_ASSERT(result == VK_SUCCESS, "Error al crear BLAS. Code: " + std::to_string(result));
	
	// Gpu address
	VkAccelerationStructureDeviceAddressInfoKHR blasGpuAddressInfo{};
	blasGpuAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	blasGpuAddressInfo.accelerationStructure = accelerationStructureHandle;

	VkDeviceOrHostAddressConstKHR blasAddress{};
	blasAddress.deviceAddress = RendererVulkan::pvkGetAccelerationStructureDeviceAddressKHR(logicalDevice, &blasGpuAddressInfo);

	bufferAddressInfo.buffer = buildBuffer->GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer();

	VkDeviceOrHostAddressConstKHR blasBuildAddress{};
	blasBuildAddress.deviceAddress = RendererVulkan::pvkGetBufferDeviceAddressKHR(logicalDevice, &bufferAddressInfo);
	
	// Build
	VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
	accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	accelerationBuildGeometryInfo.dstAccelerationStructure = accelerationStructureHandle;
	accelerationBuildGeometryInfo.geometryCount = 1;
	accelerationBuildGeometryInfo.pGeometries = &geometryInfo;
	accelerationBuildGeometryInfo.scratchData.deviceAddress = blasBuildAddress.deviceAddress;

	// Construcción final
	const VkAccelerationStructureBuildRangeInfoKHR* ranges[] = { &blasBuildRangeInfo };

	auto blasCommandList = Engine::GetRenderer()->CreateSingleUseCommandList();
	blasCommandList->Start();
	RendererVulkan::pvkCmdBuildAccelerationStructuresKHR(blasCommandList->As<CommandListVulkan>()->GetCommandBuffers()->At(blasCommandList->GetCommandListIndex()), 1, &accelerationBuildGeometryInfo, ranges);
	blasCommandList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(blasCommandList.GetPointer());

	// Final address
	VkAccelerationStructureDeviceAddressInfoKHR finalBlasAddressInfo{};
	finalBlasAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	finalBlasAddressInfo.accelerationStructure = accelerationStructureHandle;

	accelerationStructureGpuAddress.deviceAddress = RendererVulkan::pvkGetAccelerationStructureDeviceAddressKHR(logicalDevice, &finalBlasAddressInfo);
}

void BottomLevelAccelerationStructureVulkan::Update() {
	// TODO
	VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
	accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
	accelerationBuildGeometryInfo.dstAccelerationStructure = accelerationStructureHandle;
	accelerationBuildGeometryInfo.geometryCount = 1;
	accelerationBuildGeometryInfo.pGeometries = &geometryInfo;
	//accelerationBuildGeometryInfo.scratchData.deviceAddress = blasBuildAddress.deviceAddress;
}

TSize BottomLevelAccelerationStructureVulkan::GetNumTriangles() const {
	return numTriangles;
}

VkAccelerationStructureGeometryKHR BottomLevelAccelerationStructureVulkan::GetGeometryInfo() const {
	return geometryInfo;
}

VkAccelerationStructureKHR BottomLevelAccelerationStructureVulkan::GetHandle() const {
	return accelerationStructureHandle;
}

VkDeviceOrHostAddressConstKHR BottomLevelAccelerationStructureVulkan::GetGpuAddress() const {
	return accelerationStructureGpuAddress;
}

