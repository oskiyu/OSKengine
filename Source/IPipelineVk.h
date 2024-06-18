#pragma once

#include <vulkan/vulkan.h>

#include "VertexInfo.h"
#include "PipelineCreateInfo.h"

namespace OSK::GRAPHICS {

	enum class ShaderStage;


	/// @brief Contiene los datos compilados de un shader.
	struct ShaderStageVk {
		VkShaderModule shaderModule = VK_NULL_HANDLE;
		VkPipelineShaderStageCreateInfo shaderCreateInfo{};
	};


	/// @brief Clase base para todos los pipelines de Vulkan.
	/// Contiene funciones auxiliares.
	class OSKAPI_CALL IPipelineVk {

	public:

		virtual ~IPipelineVk();

		/// @return Pipeline nativo.
		VkPipeline GetPipeline() const;

	protected:

		/// @brief Crea un módulo de un shader en concreto.
		/// @param code Código SPIR-V del shader.
		/// @param name Nombre debug del shader.
		/// @param logicalDevice Logical device sobre el que se creará.
		/// @return Módulo compilado del shader.
		/// 
		/// @throws ShaderLoadingException si ocurre algún error.
		VkShaderModule CreateShaderModule(
			std::span<const char> code, 
			std::string_view name, 
			VkDevice logicalDevice);

		VkVertexInputBindingDescription GetBindingDescription(const VertexInfo& info) const;
		VkFormat GetVertexAttribFormat(const VertexInfo::Entry& entry) const;
		DynamicArray<VkVertexInputAttributeDescription> GetAttributeDescription(const VertexInfo& info) const;
		VkPipelineColorBlendAttachmentState GetColorBlendInfo(const PipelineCreateInfo& info) const;
		VkPolygonMode GetPolygonMode(PolygonMode mode) const;
		VkCullModeFlagBits GetCullMode(PolygonCullMode mode) const;
		VkFrontFace GetFrontFaceMode(PolygonFrontFaceType type) const;
		VkShaderStageFlagBits GetShaderStageVk(ShaderStage stage) const;

		ShaderStageVk LoadShader(const std::string& path, ShaderStage stage);

		DynamicArray<VkPipelineShaderStageCreateInfo> shaderStagesInfo;
		DynamicArray<VkShaderModule> shaderModulesToDelete;

		VkPipeline pipeline = VK_NULL_HANDLE;

	};

}
