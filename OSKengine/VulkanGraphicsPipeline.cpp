#include "VulkanGraphicsPipeline.h"

#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderpass.h"

namespace OSK::VULKAN {

	void VulkanGraphicsPipeline::CreateGraphicsPipeline(const VulkanDevice& device, const VulkanSwapchain& swapchain, const VulkanRenderpass& renderpass, const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
		//Carga el bytecode de los shaders
		auto vertexShader = OSK::FileIO::ReadBinaryFromFile(vertexShaderPath + ".spv");
		auto fragmentShader = OSK::FileIO::ReadBinaryFromFile(fragmentShaderPath + ".spv");

		//Crea los módulos
		VkShaderModule vertexShaderModule = CreateShaderModule(device, vertexShader);
		VkShaderModule fragmentShaderModule = CreateShaderModule(device, fragmentShader);

		//Asignar los shaders
		//struct de creación : vertex
		VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {};
		vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; //Tipo de struct
		vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; //Tipo de shader
		vertexShaderStageInfo.module = vertexShaderModule; //Módulo
		vertexShaderStageInfo.pName = "main"; //Función principal del shader
		vertexShaderStageInfo.pSpecializationInfo = NULL; //Optimizaciones

		//struct de creación : fragment
		VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {};
		fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; //Tipo de struct
		fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT; //Tipo de shader
		fragmentShaderStageInfo.module = fragmentShaderModule; //Módulo
		fragmentShaderStageInfo.pName = "main"; //Función principal del shader
		fragmentShaderStageInfo.pSpecializationInfo = NULL; //Optimizaciones

		//Almacena las asignaciones de los shaders
		VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

		//Formato de los vértices: cómo se pasan al shader
		auto bindingDescription = OSK::Vertex::GetBindingDescription();
		auto attributeDecriptions = OSK::Vertex::GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; //Tipo de struct
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDecriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDecriptions.data(); // Optional

		//Cómo se van a interpretar los vértices a la hora de renderizar
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO; //Tipo de struct

		/* Modos de topología:

			'VK_PRIMITIVE_TOPOLOGY_POINT_LIST' = puntos

			'VK_PRIMITIVE_TOPOLOGY_LINE_LIST' = línea entre dos vértices, sin indexing

			'VK_PRIMITIVE_TOPOLOGY_LINE_STRIP' = línea entre dos vértices, con indexing

			'VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST' = triángulos, sin indexing

			'VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP' = triángulos, con indexing

		*/
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; //Triángulos
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		//Viewport
		VkViewport viewport = {};
		viewport.x = 0.0f; //Coordenada inicial en X
		viewport.y = 0.0f; //Coordenada inicial en Y
		viewport.width = (float)swapchain.SwapchainExtent.width; //Ancho
		viewport.height = (float)swapchain.SwapchainExtent.height; //Alto
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		//Scissor: sólamente se renderizará algo si está dentro del objeto, sin importar si entra en el viewport o no
		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchain.SwapchainExtent;

		//Struct para crear el viewport
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO; //Tipo de struct
		viewportState.viewportCount = 1; //Número de viewports
		viewportState.pViewports = &viewport; //Viewport
		viewportState.scissorCount = 1; //Número de tijeras
		viewportState.pScissors = &scissor; //Tijeras

		//Crear el rasterizador
		//A partir de la geometría (vértices y triángulos) crea los fragmentos
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO; //Tipo de struct
		rasterizer.depthClampEnable = VK_FALSE; //Los fragmentos que están tapados por otros se ignoran
		rasterizer.rasterizerDiscardEnable = VK_FALSE; //Para comunicarse con el framebuffer

		/* Modos de polígonos:

			'VK_POLYGON_MODE_FILL' = triángulos

			'VK_POLYGON_MODE_LINE' = líneas

			'VK_POLYGON_MODE_POINT' = puntos

		*/
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL; //Cómo se generan los fragmentos
		rasterizer.lineWidth = 1.0f;

		//Culling
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		//MSAA
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO; //Tipo de struct
		multisampling.sampleShadingEnable = VK_FALSE; //Activvarlo
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = NULL; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		//Color blending
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; //Tipo de struct
		colorBlendAttachment.blendEnable = VK_FALSE; //Activarlo
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		//Depth stencil
		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional

		//Bitwise color blending
		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO; //Tipo de struct
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		//Crea el PipelineLayout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; //Tipo de struct
		pipelineLayoutInfo.setLayoutCount = 1; // Optional
		pipelineLayoutInfo.pSetLayouts = &swapchain.DescriptorSetLayout; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

		VkResult result = vkCreatePipelineLayout(device.LogicalDevice, &pipelineLayoutInfo, NULL, &PipelineLayout);

		//Error handling
		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "crear pipeline layout", __LINE__);
			throw std::runtime_error("ERROR: pipeline layout");
		}

		//struct para la creacion del pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO; //Tipo de struct
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = NULL; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = NULL; // Optional
		pipelineInfo.layout = PipelineLayout;
		pipelineInfo.renderPass = renderpass.RenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional
		pipelineInfo.pDepthStencilState = &depthStencil;

		//Crear la pipeline
		result = vkCreateGraphicsPipelines(device.LogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &GraphicsPipeline);

		//Error handling
		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "crear graphics pipeline", __LINE__);
			throw std::runtime_error("ERROR: graphics pipeline");
		}

		//clean-up: destruye los módulos (ya no son necesarios)
		vkDestroyShaderModule(device.LogicalDevice, fragmentShaderModule, NULL);
		vkDestroyShaderModule(device.LogicalDevice, vertexShaderModule, NULL);
	}


	void VulkanGraphicsPipeline::Destroy(const VulkanDevice& device) {
		//Destruir el graphics pipeline
		vkDestroyPipeline(device.LogicalDevice, GraphicsPipeline, NULL);

		//Destruir el pipeline layout
		vkDestroyPipelineLayout(device.LogicalDevice, PipelineLayout, NULL);
	}


	OSK_INFO_STATIC 
	VkShaderModule VulkanGraphicsPipeline::CreateShaderModule(const VulkanDevice& device, const std::vector<char>& code) {
		//Struct de creación del módulo
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; //Tipo de struct
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		//Crear módulo
		VkShaderModule shaderrModule;
		VkResult result = vkCreateShaderModule(device.LogicalDevice, &createInfo, NULL, &shaderrModule);

		//Error handling
		if (result != VK_SUCCESS) {
			throw std::runtime_error("ERROR: crear shader module");
		}

		//return
		return shaderrModule;
	}

}