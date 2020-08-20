#pragma once

#include "DescriptorLayout.h"
#include "DescriptorSet.h"
#include "GraphicsPipeline.h"
#include "VulkanRenderer.h"

namespace OSK {

	GraphicsPipeline* CreateGraphicsPipeline(VulkanRenderer& renderAPI, const std::string&vertexPath, const std::string& fragmentPath) {
		GraphicsPipeline* pipeline = renderAPI.CreateNewGraphicsPipeline(vertexPath, fra);
	}

}
