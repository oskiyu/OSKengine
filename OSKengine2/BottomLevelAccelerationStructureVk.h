#pragma once

#include "IBottomLevelAccelerationStructure.h"

#include "UniquePtr.hpp"
#include "IGpuDataBuffer.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL BottomLevelAccelerationStructureVk : public IBottomLevelAccelerationStructure {

	public:

		~BottomLevelAccelerationStructureVk();

		void Setup(const IGpuVertexBuffer& vertexBuffer, const IGpuIndexBuffer& indexBuffer, RtAccelerationStructureFlags flags) override;
		void Update(ICommandList* cmdList) override;

		TSize GetNumTriangles() const;
		VkAccelerationStructureGeometryKHR GetGeometryInfo() const;

		VkAccelerationStructureKHR GetHandle() const;
		VkDeviceOrHostAddressConstKHR GetGpuAddress() const;

	private:

		static VkDeviceAddress GetBufferDeviceAddress(const VkBuffer buffer, const VkDevice logicalDevice);
		static VkDeviceAddress GetBlasDeviceAddress(const VkAccelerationStructureKHR tlas, const VkDevice logicalDevice);

		UniquePtr<GpuDataBuffer> accelerationStructureBuffer;
		UniquePtr<GpuDataBuffer> buildBuffer;

		VkAccelerationStructureKHR accelerationStructureHandle = VK_NULL_HANDLE;
		VkAccelerationStructureGeometryKHR geometryInfo{};
		VkDeviceOrHostAddressConstKHR accelerationStructureGpuAddress{};

		TSize numTriangles = 0;

	};

}
