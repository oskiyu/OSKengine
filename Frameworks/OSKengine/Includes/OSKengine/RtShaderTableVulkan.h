#pragma once

#include "IRtShaderTable.h"

#include "UniquePtr.hpp"
#include "IGpuDataBuffer.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL RtShaderTableVulkan : public IRtShaderTable {

	public:

		RtShaderTableVulkan(TSize numShaderGroups, VkPipeline pipeline);

		VkStridedDeviceAddressRegionKHR GetRaygenTableAddressRegion() const;
		VkStridedDeviceAddressRegionKHR GetClosestHitTableAddressRegion() const;
		VkStridedDeviceAddressRegionKHR GetMissTableAddressRegion() const;

	private:

		VkStridedDeviceAddressRegionKHR raygenTableAddressRegion{};
		VkStridedDeviceAddressRegionKHR hitTableAddressRegion{};
		VkStridedDeviceAddressRegionKHR missTableAddressRegion{};

		UniquePtr<GpuDataBuffer> raygenShaderTable;
		UniquePtr<GpuDataBuffer> hitShaderTable;
		UniquePtr<GpuDataBuffer> missShaderTable;

	};

}
