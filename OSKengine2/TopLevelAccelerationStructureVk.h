#pragma once

#include "ITopLevelAccelerationStructure.h"

#include "UniquePtr.hpp"
#include "IGpuDataBuffer.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL TopLevelAccelerationStructureVk : public ITopLevelAccelerationStructure {

	public:

		void Setup() override;
		void Update(ICommandList* cmdList) override;

		VkAccelerationStructureKHR GetAccelerationStructure() const;

	private:

		static VkDeviceAddress GetBufferDeviceAddress(const VkBuffer buffer, const VkDevice logicalDevice);
		static VkDeviceAddress GetTlasDeviceAddress(const VkAccelerationStructureKHR tlas, const VkDevice logicalDevice);

		UniquePtr<GpuDataBuffer> accelerationStructureBuffer;
		UniquePtr<GpuDataBuffer> buildBuffer;
		UniquePtr<GpuDataBuffer> instanceBuffer;

		VkAccelerationStructureKHR blasHandle = VK_NULL_HANDLE;
		VkAccelerationStructureKHR tlasHandle = VK_NULL_HANDLE;
		VkDeviceOrHostAddressConstKHR tlasAddress{};
		VkAccelerationStructureGeometryKHR geometryInfo{};

	};

}
