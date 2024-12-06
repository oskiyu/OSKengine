#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IRtShaderTable.h"

#include "UniquePtr.hpp"
#include "GpuBuffer.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL RtShaderTableVk final : public IRtShaderTable {

	public:

		/// @throws RtShaderBindingTableCreationException Si ocurre algún error.
		RtShaderTableVk(USize32 numShaderGroups, VkPipeline pipeline);

		VkStridedDeviceAddressRegionKHR GetRaygenTableAddressRegion() const;
		VkStridedDeviceAddressRegionKHR GetClosestHitTableAddressRegion() const;
		VkStridedDeviceAddressRegionKHR GetMissTableAddressRegion() const;

	private:

		VkStridedDeviceAddressRegionKHR raygenTableAddressRegion{};
		VkStridedDeviceAddressRegionKHR hitTableAddressRegion{};
		VkStridedDeviceAddressRegionKHR missTableAddressRegion{};

		UniquePtr<GpuBuffer> raygenShaderTable;
		UniquePtr<GpuBuffer> hitShaderTable;
		UniquePtr<GpuBuffer> missShaderTable;

	};

}

#endif
