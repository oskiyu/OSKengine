#include "BottomLevelAccelerationStructureVk.h"

#include "OSKengine.h"
#include "RendererVk.h"
#include "IGpuMemoryAllocator.h"
#include "GpuMemoryTypes.h"
#include "GpuMemoryBlockVk.h"
#include "IGpuMemorySubblock.h"
#include "GpuVk.h"
#include "CommandListVk.h"

#include "AccelerationStructuresExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

BottomLevelAccelerationStructureVk::~BottomLevelAccelerationStructureVk() {
	RendererVk::pvkDestroyAccelerationStructureKHR(
		Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		accelerationStructureHandle,
		VK_NULL_HANDLE);
}

void BottomLevelAccelerationStructureVk::Setup(
	const GpuBuffer& vertexBuffer, const VertexBufferView& vertexView,
	const GpuBuffer& indexBuffer, const IndexBufferView& indexView,
	RtAccelerationStructureFlags flags) {

	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();
	IGpuMemoryAllocator* memoryAllocator = Engine::GetRenderer()->GetAllocator();

	matrixBuffer = Engine::GetRenderer()->GetAllocator()->CreateBuffer(sizeof(VkTransformMatrixKHR), 0, GpuBufferUsage::RT_ACCELERATION_STRUCTURE_BUILDING, GpuSharedMemoryType::GPU_AND_CPU).GetPointer();
	SetMatrix(glm::mat4(1.0f));

	// Obtene la localización en la memoria de la GPU de los buffers.
	const VkDeviceOrHostAddressConstKHR vertexBufferGpuAddress {
		.deviceAddress = GetBufferDeviceAddress(vertexBuffer.GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), logicalDevice)
			+ vertexBuffer.GetMemorySubblock()->GetOffsetFromBlock()
	};

	const VkDeviceOrHostAddressConstKHR indexBufferGpuAddress {
		.deviceAddress = GetBufferDeviceAddress(indexBuffer.GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), logicalDevice)
			+ indexBuffer.GetMemorySubblock()->GetOffsetFromBlock()
	};

	const VkDeviceOrHostAddressConstKHR matrixBufferGpuAddress {
		.deviceAddress = GetBufferDeviceAddress(matrixBuffer->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), logicalDevice)
			+ matrixBuffer->GetMemorySubblock()->GetOffsetFromBlock()
	};


	// Tamaño del vértice
	USize64 vertexStride = vertexView.vertexInfo.GetSize();

	// Info de la geometría
	geometryInfo = VkAccelerationStructureGeometryKHR{};

	geometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	geometryInfo.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	geometryInfo.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	geometryInfo.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	geometryInfo.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
	geometryInfo.geometry.triangles.vertexData = vertexBufferGpuAddress;
	geometryInfo.geometry.triangles.maxVertex = vertexView.numVertices;
	geometryInfo.geometry.triangles.vertexStride = vertexStride;
	geometryInfo.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
	geometryInfo.geometry.triangles.indexData = indexBufferGpuAddress;
	geometryInfo.geometry.triangles.transformData.deviceAddress = 0;
	geometryInfo.geometry.triangles.transformData.hostAddress = nullptr;
	geometryInfo.geometry.triangles.transformData = matrixBufferGpuAddress;

	numTriangles = indexView.numIndices / 3;

	// Rango único para toda la geometría
	VkAccelerationStructureBuildRangeInfoKHR blasBuildRangeInfo{};
	blasBuildRangeInfo.primitiveCount = numTriangles;
	blasBuildRangeInfo.primitiveOffset = 0;
	blasBuildRangeInfo.firstVertex = 0;
	blasBuildRangeInfo.transformOffset = 0;

	VkBuildAccelerationStructureFlagsKHR buildFlags = 0;
	if (EFTraits::HasFlag(flags, RtAccelerationStructureFlags::ALLOW_UPDATE))
		buildFlags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
	if (EFTraits::HasFlag(flags, RtAccelerationStructureFlags::FAST_BUILD))
		buildFlags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;
	if (EFTraits::HasFlag(flags, RtAccelerationStructureFlags::FAST_TRACE))
		buildFlags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;

	// Obtenemos la cantidad de memoria necesaria
	VkAccelerationStructureBuildGeometryInfoKHR blasBuildGeometryInfo{};
	blasBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	blasBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	blasBuildGeometryInfo.flags = buildFlags;
	blasBuildGeometryInfo.geometryCount = 1;
	blasBuildGeometryInfo.pGeometries = &geometryInfo;

	VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
	sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	RendererVk::pvkGetAccelerationStructureBuildSizesKHR(
		logicalDevice,
		VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
		&blasBuildGeometryInfo,
		&numTriangles,
		&sizeInfo);

	accelerationStructureBuffer = memoryAllocator->CreateBuffer(sizeInfo.accelerationStructureSize, 256, GpuBufferUsage::RT_ACCELERATION_STRUCTURE, GpuSharedMemoryType::GPU_ONLY).GetPointer();
	buildBuffer = memoryAllocator->CreateBuffer(sizeInfo.buildScratchSize, Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetInfo().rtAccelerationStructuresProperites.minAccelerationStructureScratchOffsetAlignment, GpuBufferUsage::RT_ACCELERATION_STRUCTURE_BUILDING, GpuSharedMemoryType::GPU_AND_CPU).GetPointer();

	// Creación
	VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo{};
	accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	accelerationStructureCreateInfo.buffer = accelerationStructureBuffer->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer();
	accelerationStructureCreateInfo.size = sizeInfo.accelerationStructureSize;
	accelerationStructureCreateInfo.offset = accelerationStructureBuffer->GetMemorySubblock()->GetOffsetFromBlock();
	accelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	
	VkResult result = RendererVk::pvkCreateAccelerationStructureKHR(logicalDevice, &accelerationStructureCreateInfo, nullptr, &accelerationStructureHandle);
	OSK_ASSERT(result == VK_SUCCESS, AccelerationStructureCreationException(result));
	
	// Gpu address
	VkAccelerationStructureDeviceAddressInfoKHR blasGpuAddressInfo{};
	blasGpuAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	blasGpuAddressInfo.accelerationStructure = accelerationStructureHandle;

	const VkDeviceOrHostAddressConstKHR blasAddress {
		.deviceAddress = GetBlasDeviceAddress(accelerationStructureHandle, logicalDevice)
	};

	const VkDeviceOrHostAddressConstKHR blasBuildAddress {
		.deviceAddress = GetBufferDeviceAddress(buildBuffer->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), logicalDevice)
			+ buildBuffer->GetMemorySubblock()->GetOffsetFromBlock()
	};
	
	// Build
	VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
	accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
	accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	accelerationBuildGeometryInfo.dstAccelerationStructure = accelerationStructureHandle;
	accelerationBuildGeometryInfo.geometryCount = 1;
	accelerationBuildGeometryInfo.pGeometries = &geometryInfo;
	accelerationBuildGeometryInfo.scratchData.deviceAddress = blasBuildAddress.deviceAddress;

	// Construcción final
	const VkAccelerationStructureBuildRangeInfoKHR* ranges[] = { &blasBuildRangeInfo };

	auto blasCommandList = Engine::GetRenderer()->CreateSingleUseCommandList();
	blasCommandList->Start();
	RendererVk::pvkCmdBuildAccelerationStructuresKHR(blasCommandList->As<CommandListVk>()->GetCommandBuffers()->At(blasCommandList->_GetCommandListIndex()), 1, &accelerationBuildGeometryInfo, ranges);
	blasCommandList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(blasCommandList.GetPointer());

	// Final address
	VkAccelerationStructureDeviceAddressInfoKHR finalBlasAddressInfo{};
	finalBlasAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	finalBlasAddressInfo.accelerationStructure = accelerationStructureHandle;

	accelerationStructureGpuAddress.deviceAddress = RendererVk::pvkGetAccelerationStructureDeviceAddressKHR(logicalDevice, &finalBlasAddressInfo);
}

void BottomLevelAccelerationStructureVk::Update(ICommandList* cmdList) {
	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();

	// TODO
	VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
	accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
	accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
	accelerationBuildGeometryInfo.srcAccelerationStructure = accelerationStructureHandle;
	accelerationBuildGeometryInfo.dstAccelerationStructure = accelerationStructureHandle;
	accelerationBuildGeometryInfo.geometryCount = 1;
	accelerationBuildGeometryInfo.pGeometries = &geometryInfo;
	accelerationBuildGeometryInfo.scratchData.deviceAddress = GetBufferDeviceAddress(buildBuffer->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), logicalDevice) + buildBuffer->GetMemorySubblock()->GetOffsetFromBlock();

	// Rango único para toda la geometría
	VkAccelerationStructureBuildRangeInfoKHR blasBuildRangeInfo{};
	blasBuildRangeInfo.primitiveCount = numTriangles;
	blasBuildRangeInfo.primitiveOffset = 0;
	blasBuildRangeInfo.firstVertex = 0;
	blasBuildRangeInfo.transformOffset = 0;

	// Construcción final
	const VkAccelerationStructureBuildRangeInfoKHR* ranges[] = { &blasBuildRangeInfo };

	const VkCommandBuffer vkCmdList = cmdList->As<CommandListVk>()->GetCommandBuffers()->At(cmdList->_GetCommandListIndex());
	RendererVk::pvkCmdBuildAccelerationStructuresKHR(vkCmdList, 1, &accelerationBuildGeometryInfo, ranges);

	// Sincronización para que no se pueda usar el blas hasta que haya sido reconstruido correctamente.
	VkMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
	barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

	vkCmdPipelineBarrier(vkCmdList, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
		VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, 0, 1, &barrier, 0, nullptr, 0, nullptr);
}

USize32 BottomLevelAccelerationStructureVk::GetNumTriangles() const {
	return numTriangles;
}

VkAccelerationStructureGeometryKHR BottomLevelAccelerationStructureVk::GetGeometryInfo() const {
	return geometryInfo;
}

VkAccelerationStructureKHR BottomLevelAccelerationStructureVk::GetHandle() const {
	return accelerationStructureHandle;
}

VkDeviceOrHostAddressConstKHR BottomLevelAccelerationStructureVk::GetGpuAddress() const {
	return accelerationStructureGpuAddress;
}


VkDeviceAddress BottomLevelAccelerationStructureVk::GetBufferDeviceAddress(const VkBuffer buffer, const VkDevice logicalDevice) {
	VkBufferDeviceAddressInfoKHR bufferAddressInfo{};
	bufferAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	bufferAddressInfo.buffer = buffer;

	return RendererVk::pvkGetBufferDeviceAddressKHR(logicalDevice, &bufferAddressInfo);
}

VkDeviceAddress BottomLevelAccelerationStructureVk::GetBlasDeviceAddress(const VkAccelerationStructureKHR tlas, const VkDevice logicalDevice) {
	VkAccelerationStructureDeviceAddressInfoKHR finalTlasGpuAddressInfo{};
	finalTlasGpuAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	finalTlasGpuAddressInfo.accelerationStructure = tlas;

	return RendererVk::pvkGetAccelerationStructureDeviceAddressKHR(logicalDevice, &finalTlasGpuAddressInfo);
}
