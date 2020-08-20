#pragma once

#include "OSKmacros.h"

#include <vulkan/vulkan.h>

#include "PointLight.h"
#include "DirectionalLight.h"

#include <vector>
#include "VulkanBuffer.h"

namespace OSK {

	class LightUBO {

	public:

		DirectionalLight Directional{};
		std::vector<PointLight> Points{};

		inline const size_t PointsSize() const {
			return Points.size() * sizeof(PointLight);
		}

		inline const size_t Size() const {
			return sizeof(DirectionalLight) + PointsSize();
		}

		void UpdateBuffer(VkDevice logicalDevice, VulkanBuffer& buffer) const;

	};

}