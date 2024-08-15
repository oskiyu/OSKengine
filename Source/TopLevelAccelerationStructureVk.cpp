#include "TopLevelAccelerationStructureVk.h"

#include "BottomLevelAccelerationStructureVk.h"
#include "OSKengine.h"
#include "RendererVk.h"
#include "IGpuMemoryAllocator.h"
#include "GpuMemoryTypes.h"
#include "GpuMemoryBlockVk.h"
#include "GpuMemorySubblockVk.h"
#include "GpuVk.h"
#include "CommandListVk.h"

#include "AccelerationStructuresExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

TopLevelAccelerationStructureVk::~TopLevelAccelerationStructureVk() {
	RendererVk::pvkDestroyAccelerationStructureKHR(
		Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		tlasHandle,
		VK_NULL_HANDLE);
}

void TopLevelAccelerationStructureVk::Setup() {
	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();
	IGpuMemoryAllocator* memoryAllocator = Engine::GetRenderer()->GetAllocator();

	const VkTransformMatrixKHR transform {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	};

	DynamicArray<VkAccelerationStructureInstanceKHR> instances = DynamicArray<VkAccelerationStructureInstanceKHR>::CreateResized(blass.GetSize());
	for (UIndex32 i = 0; i < blass.GetSize(); i++) {
		instances[i].transform = transform;
		instances[i].instanceCustomIndex = static_cast<uint32_t>(i);
		instances[i].mask = 0xFF;
		instances[i].instanceShaderBindingTableRecordOffset = 0;
		instances[i].flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR | VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR;
		instances[i].accelerationStructureReference = blass[i]->As<BottomLevelAccelerationStructureVk>()->GetGpuAddress().deviceAddress;
	}

	// Geometría única del TLAS
	instanceBuffer = memoryAllocator->CreateBuffer(
		sizeof(VkAccelerationStructureInstanceKHR) * instances.GetSize(), 
		GPU_MEMORY_NO_ALIGNMENT,
		GpuBufferUsage::RT_ACCELERATION_STRUCTURE_BUILDING, 
		GpuSharedMemoryType::GPU_AND_CPU,
		GpuQueueType::MAIN).GetPointer();

	if (!instances.IsEmpty()) {
		instanceBuffer->MapMemory();
		instanceBuffer->Write(instances.GetData(), sizeof(VkAccelerationStructureInstanceKHR) * instances.GetSize());
		instanceBuffer->Unmap();
	}

	const VkDeviceOrHostAddressConstKHR instancesAddress {
		.deviceAddress = GetBufferDeviceAddress(instanceBuffer->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), logicalDevice)
			+ instanceBuffer->GetMemorySubblock()->GetOffsetFromBlock()
	};


	geometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	geometryInfo.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	geometryInfo.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	geometryInfo.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	geometryInfo.geometry.instances.arrayOfPointers = VK_FALSE;
	geometryInfo.geometry.instances.data = instancesAddress;

	// Obtenemos el tamaño necesitado.
	VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
	accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	accelerationStructureBuildGeometryInfo.geometryCount = 1;
	accelerationStructureBuildGeometryInfo.pGeometries = &geometryInfo;

	const UIndex32 one = 1;
	VkAccelerationStructureBuildSizesInfoKHR tlasSizeInfo{};
	tlasSizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	RendererVk::pvkGetAccelerationStructureBuildSizesKHR(
		logicalDevice,
		VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
		&accelerationStructureBuildGeometryInfo,
		&one,
		&tlasSizeInfo);

	accelerationStructureBuffer = memoryAllocator->CreateBuffer(
		tlasSizeInfo.accelerationStructureSize, 
		256, 
		GpuBufferUsage::RT_ACCELERATION_STRUCTURE, 
		GpuSharedMemoryType::GPU_ONLY,
		GpuQueueType::MAIN).GetPointer();

	// Creación
	VkAccelerationStructureCreateInfoKHR asCreateInfo{};
	asCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	asCreateInfo.buffer = accelerationStructureBuffer->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer();
	asCreateInfo.size = tlasSizeInfo.accelerationStructureSize;
	asCreateInfo.offset = accelerationStructureBuffer->GetMemorySubblock()->GetOffsetFromBlock();
	asCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	VkResult result = RendererVk::pvkCreateAccelerationStructureKHR(logicalDevice, &asCreateInfo, nullptr, &tlasHandle);
	OSK_ASSERT(result == VK_SUCCESS, AccelerationStructureCreationException(result));

	// Address
	VkAccelerationStructureDeviceAddressInfoKHR tlasGpuAddressInfo{};
	tlasGpuAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	tlasGpuAddressInfo.accelerationStructure = tlasHandle;

	// Construcción
	buildBuffer = memoryAllocator->CreateBuffer(
		tlasSizeInfo.buildScratchSize, 
		GPU_MEMORY_NO_ALIGNMENT,
		GpuBufferUsage::RT_ACCELERATION_STRUCTURE_BUILDING, 
		GpuSharedMemoryType::GPU_AND_CPU,
		GpuQueueType::MAIN).GetPointer();

	const VkDeviceOrHostAddressConstKHR tlasBuildAddress {
		.deviceAddress = GetBufferDeviceAddress(buildBuffer->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), logicalDevice)
			+ buildBuffer->GetMemorySubblock()->GetOffsetFromBlock()
	};

	VkAccelerationStructureBuildGeometryInfoKHR tlasBuildGeometryInfo{};
	tlasBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	tlasBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	tlasBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
	tlasBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	tlasBuildGeometryInfo.dstAccelerationStructure = tlasHandle;
	tlasBuildGeometryInfo.geometryCount = 1;
	tlasBuildGeometryInfo.pGeometries = &geometryInfo;
	tlasBuildGeometryInfo.scratchData.deviceAddress = tlasBuildAddress.deviceAddress;

	VkAccelerationStructureBuildRangeInfoKHR tlasBuildRangeInfo{};
	tlasBuildRangeInfo.primitiveCount = static_cast<uint32_t>(instances.GetSize());
	tlasBuildRangeInfo.primitiveOffset = 0;
	tlasBuildRangeInfo.firstVertex = 0;
	tlasBuildRangeInfo.transformOffset = 0;

	DynamicArray<VkAccelerationStructureBuildRangeInfoKHR*> tlasRanges = {
		&tlasBuildRangeInfo
	};

	auto tlasCommandList = Engine::GetRenderer()->CreateSingleUseCommandList(GpuQueueType::MAIN);
	tlasCommandList->Start();
	RendererVk::pvkCmdBuildAccelerationStructuresKHR(tlasCommandList->As<CommandListVk>()->GetCommandBuffers()[tlasCommandList->_GetCommandListIndex()], 1, &tlasBuildGeometryInfo, tlasRanges.GetData());
	tlasCommandList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(tlasCommandList.GetPointer());

	tlasAddress.deviceAddress = GetTlasDeviceAddress(tlasHandle, logicalDevice);
}

void TopLevelAccelerationStructureVk::Update(ICommandList* cmdList) {
	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();
	auto mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;

	if (needsRebuild) {
		const VkTransformMatrixKHR transform{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f
		};

		DynamicArray<VkAccelerationStructureInstanceKHR> instances = DynamicArray<VkAccelerationStructureInstanceKHR>::CreateResized(blass.GetSize());
		for (UIndex32 i = 0; i < blass.GetSize(); i++) {
			instances[i].transform = transform;
			instances[i].instanceCustomIndex = i;
			instances[i].mask = 0xFF;
			instances[i].instanceShaderBindingTableRecordOffset = 0;
			instances[i].flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR | VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR;
			instances[i].accelerationStructureReference = blass[i]->As<BottomLevelAccelerationStructureVk>()->GetGpuAddress().deviceAddress;
		}

		instanceBuffer = Engine::GetRenderer()->GetAllocator()->CreateBuffer(
			sizeof(VkAccelerationStructureInstanceKHR) * instances.GetSize(), 
			GPU_MEMORY_NO_ALIGNMENT,
			GpuBufferUsage::RT_ACCELERATION_STRUCTURE_BUILDING, 
			GpuSharedMemoryType::GPU_AND_CPU,
			GpuQueueType::MAIN).GetPointer();

		instanceBuffer->MapMemory();
		instanceBuffer->Write(instances.GetData(), sizeof(VkAccelerationStructureInstanceKHR) * instances.GetSize());
		instanceBuffer->Unmap();

		const VkDeviceOrHostAddressConstKHR instancesAddress{
			.deviceAddress = GetBufferDeviceAddress(instanceBuffer->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), logicalDevice)
				+ instanceBuffer->GetMemorySubblock()->GetOffsetFromBlock()
		};

		geometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometryInfo.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
		geometryInfo.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		geometryInfo.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		geometryInfo.geometry.instances.arrayOfPointers = VK_FALSE;
		geometryInfo.geometry.instances.data = instancesAddress;

		mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

		needsRebuild = false;
	}

	
	VkAccelerationStructureBuildGeometryInfoKHR tlasBuildGeometryInfo{};
	tlasBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	tlasBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	tlasBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
	tlasBuildGeometryInfo.mode = mode;
	tlasBuildGeometryInfo.srcAccelerationStructure = tlasHandle;
	tlasBuildGeometryInfo.dstAccelerationStructure = tlasHandle;
	tlasBuildGeometryInfo.geometryCount = 1;
	tlasBuildGeometryInfo.pGeometries = &geometryInfo;
	tlasBuildGeometryInfo.scratchData.deviceAddress = GetBufferDeviceAddress(buildBuffer->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), logicalDevice) + buildBuffer->GetMemorySubblock()->GetOffsetFromBlock();

	VkAccelerationStructureBuildRangeInfoKHR tlasBuildRangeInfo{};
	tlasBuildRangeInfo.primitiveCount = static_cast<uint32_t>(blass.GetSize());
	tlasBuildRangeInfo.primitiveOffset = 0;
	tlasBuildRangeInfo.firstVertex = 0;
	tlasBuildRangeInfo.transformOffset = 0;

	DynamicArray<VkAccelerationStructureBuildRangeInfoKHR*> tlasRanges = {
		&tlasBuildRangeInfo
	};

	const VkCommandBuffer vkCmdList = cmdList->As<CommandListVk>()->GetCommandBuffers()[cmdList->_GetCommandListIndex()];
	RendererVk::pvkCmdBuildAccelerationStructuresKHR(vkCmdList, 1, &tlasBuildGeometryInfo, tlasRanges.GetData());

	// Sincronización para que no se pueda usar el blas hasta que haya sido reconstruido correctamente.
	VkMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
	barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

	vkCmdPipelineBarrier(vkCmdList, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
		VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 0, 1, &barrier, 0, nullptr, 0, nullptr);
	

	VkAccelerationStructureDeviceAddressInfoKHR finalTlasGpuAddressInfo{};
	finalTlasGpuAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	finalTlasGpuAddressInfo.accelerationStructure = tlasHandle;

	tlasAddress.deviceAddress = GetTlasDeviceAddress(tlasHandle, logicalDevice);
}

VkAccelerationStructureKHR TopLevelAccelerationStructureVk::GetAccelerationStructure() const {
	return tlasHandle;
}

VkDeviceAddress TopLevelAccelerationStructureVk::GetBufferDeviceAddress(const VkBuffer buffer, const VkDevice logicalDevice) {
	VkBufferDeviceAddressInfoKHR bufferAddressInfo{};
	bufferAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	bufferAddressInfo.buffer = buffer;

	return RendererVk::pvkGetBufferDeviceAddressKHR(logicalDevice, &bufferAddressInfo);
}

VkDeviceAddress TopLevelAccelerationStructureVk::GetTlasDeviceAddress(const VkAccelerationStructureKHR tlas, const VkDevice logicalDevice) {
	VkAccelerationStructureDeviceAddressInfoKHR finalTlasGpuAddressInfo{};
	finalTlasGpuAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	finalTlasGpuAddressInfo.accelerationStructure = tlas;

	return RendererVk::pvkGetAccelerationStructureDeviceAddressKHR(logicalDevice, &finalTlasGpuAddressInfo);
}
