#include "TopLevelAccelerationStructureVulkan.h"

#include "BottomLevelAccelerationStructureVulkan.h"
#include "OSKengine.h"
#include "RendererVulkan.h"
#include "IGpuMemoryAllocator.h"
#include "GpuMemoryTypes.h"
#include "GpuMemoryBlockVulkan.h"
#include "GpuMemorySubblockVulkan.h"
#include "GpuVulkan.h"
#include "CommandListVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void TopLevelAccelerationStructureVulkan::Setup() {
	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
	IGpuMemoryAllocator* memoryAllocator = Engine::GetRenderer()->GetMemoryAllocator();

	// Obtene la localización en la memoria de la GPU de los buffers.
	VkBufferDeviceAddressInfoKHR bufferAddressInfo{};
	bufferAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;

	const VkTransformMatrixKHR transform{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	};

	DynamicArray<VkAccelerationStructureInstanceKHR> instances = DynamicArray<VkAccelerationStructureInstanceKHR>::CreateReservedArray(blass.GetSize());
	for (const auto& i : blass) {
		VkAccelerationStructureInstanceKHR accelerationStructureInstance{};
		accelerationStructureInstance.transform = transform;
		accelerationStructureInstance.instanceCustomIndex = 0;
		accelerationStructureInstance.mask = 0xFF;
		accelerationStructureInstance.instanceShaderBindingTableRecordOffset = 0;
		accelerationStructureInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		accelerationStructureInstance.accelerationStructureReference = i->As<BottomLevelAccelerationStructureVulkan>()->GetGpuAddress().deviceAddress;

		instances.Insert(accelerationStructureInstance);
	}

	// Geometría única del TLAS
	OwnedPtr<GpuDataBuffer> instanceBuffer = memoryAllocator->CreateBuffer(sizeof(VkAccelerationStructureInstanceKHR) * instances.GetSize(), GpuBufferUsage::RT_ACCELERATION_STRUCTURE_BUILDING, GpuSharedMemoryType::GPU_AND_CPU);
	instanceBuffer->MapMemory();
	instanceBuffer->Write(instances.GetData(), sizeof(VkAccelerationStructureInstanceKHR) * instances.GetSize());
	instanceBuffer->Unmap();
	bufferAddressInfo.buffer = instanceBuffer->GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer();

	VkDeviceOrHostAddressConstKHR instancesAddress{};
	instancesAddress.deviceAddress = RendererVulkan::pvkGetBufferDeviceAddressKHR(logicalDevice, &bufferAddressInfo);


	VkAccelerationStructureGeometryKHR tlasGeometry{};
	tlasGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	tlasGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	tlasGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	tlasGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	tlasGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
	tlasGeometry.geometry.instances.data = instancesAddress;

	// Obtenemos el tamaño necesitado.
	VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
	accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	accelerationStructureBuildGeometryInfo.geometryCount = 1;
	accelerationStructureBuildGeometryInfo.pGeometries = &tlasGeometry;

	const TSize one = 1;
	VkAccelerationStructureBuildSizesInfoKHR tlasSizeInfo{};
	tlasSizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	RendererVulkan::pvkGetAccelerationStructureBuildSizesKHR(
		logicalDevice,
		VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
		&accelerationStructureBuildGeometryInfo,
		&one,
		&tlasSizeInfo);

	OwnedPtr<GpuDataBuffer> tlasBuffer = memoryAllocator->CreateBuffer(tlasSizeInfo.accelerationStructureSize, GpuBufferUsage::RT_ACCELERATION_STRUCTURE, GpuSharedMemoryType::GPU_ONLY);

	// Creación
	VkAccelerationStructureCreateInfoKHR asCreateInfo{};
	asCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	asCreateInfo.buffer = tlasBuffer->GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer();
	asCreateInfo.size = tlasSizeInfo.accelerationStructureSize;
	asCreateInfo.offset = tlasBuffer->GetMemorySubblock()->GetOffsetFromBlock();
	asCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	VkResult result = RendererVulkan::pvkCreateAccelerationStructureKHR(logicalDevice, &asCreateInfo, nullptr, &tlasHandle);
	OSK_ASSERT(result == VK_SUCCESS, "Error al crear TLAS. Code: " + std::to_string(result));

	// Address
	VkAccelerationStructureDeviceAddressInfoKHR tlasGpuAddressInfo{};
	tlasGpuAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	tlasGpuAddressInfo.accelerationStructure = tlasHandle;

	VkDeviceOrHostAddressConstKHR tlasAddress{};
	tlasAddress.deviceAddress = RendererVulkan::pvkGetAccelerationStructureDeviceAddressKHR(logicalDevice, &tlasGpuAddressInfo);
	
	// Construcción
	UniquePtr<GpuDataBuffer> buildBuffer = memoryAllocator->CreateBuffer(tlasSizeInfo.buildScratchSize, GpuBufferUsage::RT_ACCELERATION_STRUCTURE_BUILDING, GpuSharedMemoryType::GPU_AND_CPU).GetPointer();

	bufferAddressInfo.buffer = buildBuffer->GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer();

	VkDeviceOrHostAddressConstKHR tlasBuildAddress{};
	tlasBuildAddress.deviceAddress = RendererVulkan::pvkGetBufferDeviceAddressKHR(logicalDevice, &bufferAddressInfo);

	VkAccelerationStructureBuildGeometryInfoKHR tlasBuildGeometryInfo{};
	tlasBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	tlasBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	tlasBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	tlasBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	tlasBuildGeometryInfo.dstAccelerationStructure = tlasHandle;
	tlasBuildGeometryInfo.geometryCount = 1;
	tlasBuildGeometryInfo.pGeometries = &tlasGeometry;
	tlasBuildGeometryInfo.scratchData.deviceAddress = tlasBuildAddress.deviceAddress;

	VkAccelerationStructureBuildRangeInfoKHR tlasBuildRangeInfo{};
	tlasBuildRangeInfo.primitiveCount = instances.GetSize();
	tlasBuildRangeInfo.primitiveOffset = 0;
	tlasBuildRangeInfo.firstVertex = 0;
	tlasBuildRangeInfo.transformOffset = 0;

	DynamicArray<VkAccelerationStructureBuildRangeInfoKHR*> tlasRanges = {
		&tlasBuildRangeInfo
	};

	auto tlasCommandList = Engine::GetRenderer()->CreateSingleUseCommandList();
	tlasCommandList->Start();
	RendererVulkan::pvkCmdBuildAccelerationStructuresKHR(tlasCommandList->As<CommandListVulkan>()->GetCommandBuffers()->At(tlasCommandList->GetCommandListIndex()), 1, &tlasBuildGeometryInfo, tlasRanges.GetData());
	tlasCommandList->Close();
	Engine::GetRenderer()->SubmitSingleUseCommandList(tlasCommandList.GetPointer());

	VkAccelerationStructureDeviceAddressInfoKHR finalTlasGpuAddressInfo{};
	finalTlasGpuAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	finalTlasGpuAddressInfo.accelerationStructure = tlasHandle;

	VkDeviceOrHostAddressKHR finalTlasAddress{};
	finalTlasAddress.deviceAddress = RendererVulkan::pvkGetAccelerationStructureDeviceAddressKHR(logicalDevice, &finalTlasGpuAddressInfo);
}

VkAccelerationStructureKHR TopLevelAccelerationStructureVulkan::GetAccelerationStructure() const {
	return tlasHandle;
}
