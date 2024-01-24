#pragma once

#include "ITopLevelAccelerationStructure.h"

#include "UniquePtr.hpp"
#include "GpuBuffer.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL TopLevelAccelerationStructureVk : public ITopLevelAccelerationStructure {

	public:

		~TopLevelAccelerationStructureVk();

		void Setup() override;
		void Update(ICommandList* cmdList) override;

		VkAccelerationStructureKHR GetAccelerationStructure() const;

	private:

		static VkDeviceAddress GetBufferDeviceAddress(const VkBuffer buffer, const VkDevice logicalDevice);
		static VkDeviceAddress GetTlasDeviceAddress(const VkAccelerationStructureKHR tlas, const VkDevice logicalDevice);

		UniquePtr<GpuBuffer> accelerationStructureBuffer;
		UniquePtr<GpuBuffer> buildBuffer;
		UniquePtr<GpuBuffer> instanceBuffer;

		VkAccelerationStructureKHR blasHandle = VK_NULL_HANDLE;
		VkAccelerationStructureKHR tlasHandle = VK_NULL_HANDLE;
		VkDeviceOrHostAddressConstKHR tlasAddress{};
		VkAccelerationStructureGeometryKHR geometryInfo{};

	};

}
