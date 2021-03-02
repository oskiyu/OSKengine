#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>
#include <string>

#include "Vertex.h"
#include "Vector4.hpp"
#include <vector>

#include "OSKtypes.h"
#include "Texture.h"
#include "DescriptorSet.h"

namespace OSK::VULKAN {
	class Renderpass;
}


namespace OSK {

	class DescriptorSet;
	
	//Represnta un graphics pipeline.
	class OSKAPI_CALL GraphicsPipeline {

		friend class RenderAPI;
		friend class RenderizableScene;

	public:

		//Destruye el graphics pipeline.
		~GraphicsPipeline();

		//Establece el viewport del pipeline.
		//	<size>: tamaño de la imagen rederizada del pipeline.
		//	<depthMinMax>: mínimo y máximo de la profundidad de la imagen.
		void SetViewport(const Vector4& size, const Vector2& depthMinMax = { 0.0f, 1.0f });

		//Establece las opciones de rasterizado.
		//	<renderObjectsOutsideRange>: true si se van a renderizar elementos que no saldrán en la imagen final.
		//	<polygonMode>: modo de pilígono (fill, lines, etc.).
		//	<cullMode>: modo de culling de los triángulos.
		//	<frontFaceType>: cúal es el frente del triángulo.
		void SetRasterizer(VkBool32 renderObjectsOutsideRange, VkPolygonMode polygonMode, VkCullModeFlagBits cullMode, VkFrontFace frontFaceType);

		//Establece si se va a usar MSAA  o no.
		//	<use>: true si se va a usar.
		//	<samples: número de samples de MSAA.
		void SetMSAA(VkBool32 use, VkSampleCountFlagBits samples);

		//Establece si se va a usar información de depth/stencil.
		void SetDepthStencil(bool use); 

		//Establece la configuración de push constants.
		//	<shaderStage>: shader en el que se va a usar el push constant.
		//	<size>: tamaño en bytes del push constant.
		//	<offset>: offset.
		void SetPushConstants(VkShaderStageFlagBits shaderStage, uint32_t size, uint32_t offset = 0);

		//Establece el layout que va a seguir el pipeline.
		//	<layout>: layout.
		void SetLayout(VkDescriptorSetLayout* layout);

		//Crea el pipeline.
		//	<renderpass>: renderpass sobre el que se va a usar el pipeline.
		OskResult Create(VULKAN::Renderpass* renderpass);

		//Enlaza este pipeline para su uso.
		//	<commandBuffer>: commandbuffer.
		void Bind(VkCommandBuffer commandBuffer) const;

		//Recarga los shaders.
		OskResult ReloadShaders();

		VkPipeline VulkanPipeline;
		VkPipelineLayout VulkanPipelineLayout;

	private:

		GraphicsPipeline(VkDevice logicalDevice, const std::string& vertexPath, const std::string& indexPath);

		bool viewportHasBeenSet = false;
		bool rasterizerHasBeenSet = false;
		bool msaaHasBeenSet = false;
		bool depthStencilHasBeenSet = false;
		bool pushConstantsHaveBeenSet = false;
		bool layoutHasBeenSet = false;

		std::string vertexPath;
		std::string fragmentPath;

		ModelTexture* DefaultTexture = nullptr;

		VkShaderModule createShaderModule(const std::vector<char>& code) const;

		OskResult loadShaders();

		void clearShaders();

		VkDevice logicalDevice;

		VkPipelineShaderStageCreateInfo shaderStages[2] = {};
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		VkViewport viewport{};
		VkRect2D scissor{};
		VkPipelineViewportStateCreateInfo viewportState{};

		VkPipelineRasterizationStateCreateInfo rasterizer{};

		VkPipelineMultisampleStateCreateInfo multisampling{};

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
		VkPipelineColorBlendStateCreateInfo colorBlending{};

		VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};

		std::vector<VkPushConstantRange> pushConstRanges{};
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

		VkDescriptorSetLayout* descriptorSetLayout;

		VkGraphicsPipelineCreateInfo pipelineInfo{};

		VkShaderModule vertexShaderModule;
		VkShaderModule fragmentShaderModule;

		VkVertexInputBindingDescription vertexBindingDesc;
		std::array<VkVertexInputAttributeDescription, OSK_VERTEX_ATTRIBUTES_COUNT> vertexInputDesc;

		VULKAN::Renderpass* targetRenderpass = nullptr;

	};

}