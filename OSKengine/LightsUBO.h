#pragma once

#include <vulkan/vulkan.h>

#include "PointLight.h"
#include "DirectionalLight.h"

#include <vector>
#include "VulkanBuffer.h"

namespace OSK {

	struct LightUBO {

		DirectionalLight Directional{};
		std::vector<PointLight> Points{};

		void* data = nullptr;
		bool isInit = false;

		inline const size_t Size() const {
			return sizeof(DirectionalLight) + Points.size() * sizeof(PointLight);
		}

		void UpdateBuffer(VulkanBuffer& buffer) {

		}

		const void* GetData() const {
			return data;
		}

	};

}