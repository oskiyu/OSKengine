#pragma once

#include "IBottomLevelAccelerationStructure.h"

#include "UniquePtr.hpp"
#include "GpuBuffer.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL BottomLevelAccelerationStructureVk : public IBottomLevelAccelerationStructure {

	public:

		~BottomLevelAccelerationStructureVk();

		void Setup(
			const GpuBuffer& vertexBuffer, 
			const VertexBufferView& vertexView,
			const GpuBuffer& indexBuffer, 
			const IndexBufferView& indexView,
			RtAccelerationStructureFlags flags) override;
		void Update(ICommandList* cmdList) override;

		USize32 GetNumTriangles() const;
		VkAccelerationStructureGeometryKHR GetGeometryInfo() const;

		VkAccelerationStructureKHR GetHandle() const;
		VkDeviceOrHostAddressConstKHR GetGpuAddress() const;

	private:

		static VkDeviceAddress GetBufferDeviceAddress(const VkBuffer buffer, const VkDevice logicalDevice);
		static VkDeviceAddress GetBlasDeviceAddress(const VkAccelerationStructureKHR tlas, const VkDevice logicalDevice);

		UniquePtr<GpuBuffer> accelerationStructureBuffer;
		UniquePtr<GpuBuffer> buildBuffer;

		VkAccelerationStructureKHR accelerationStructureHandle = VK_NULL_HANDLE;
		VkAccelerationStructureGeometryKHR geometryInfo{};
		VkDeviceOrHostAddressConstKHR accelerationStructureGpuAddress{};

		USize32 numTriangles = 0;

	};

}
