#include "GraphicsPipeline.h"

#include "FileIO.h"
#include "Log.h"
#include "Renderpass.h"
#include "DescriptorSet.h"

namespace OSK {

	GraphicsPipeline::GraphicsPipeline(VkDevice logicalDevice, const std::string& vertexPath, const std::string& fragmentPath) {
		this->logicalDevice = logicalDevice;

		this->vertexPath = vertexPath;
		this->fragmentPath = fragmentPath;

		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState cb0{};

		cb0.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		cb0.blendEnable = VK_TRUE;
		cb0.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		cb0.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		cb0.colorBlendOp = VK_BLEND_OP_ADD;
		cb0.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		cb0.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		cb0.alphaBlendOp = VK_BLEND_OP_ADD;

		colorBlendAttachments.push_back(cb0);

		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = colorBlendAttachments.size();
		colorBlending.pAttachments = colorBlendAttachments.data();
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;
	}

	GraphicsPipeline::~GraphicsPipeline() {
		clearShaders();

		if (VulkanPipeline != VK_NULL_HANDLE) {
			vkDestroyPipeline(logicalDevice, VulkanPipeline, nullptr);
			VulkanPipeline = VK_NULL_HANDLE;
		}
		if (VulkanPipelineLayout != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(logicalDevice, VulkanPipelineLayout, nullptr);
			VulkanPipelineLayout = VK_NULL_HANDLE;
		}
	}

	void GraphicsPipeline::SetViewport(const Vector4& size, const Vector2& depthMinMax) {
		viewport.x = size.X;
		viewport.y = size.Y;
		viewport.width = size.Z;
		viewport.height = size.W;
		viewport.minDepth = depthMinMax.X;
		viewport.maxDepth = depthMinMax.Y;

		scissor.offset = { 0,0 };
		scissor.extent = { (uint32_t)size.Z, (uint32_t)size.W };

		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		viewportHasBeenSet = true;
	}

	void GraphicsPipeline::SetRasterizer(VkBool32 renderObjectsOutsideRange, VkPolygonMode polygonMode, VkCullModeFlagBits cullMode, VkFrontFace frontFaceType) {
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = renderObjectsOutsideRange; //FALSE: si el objeto está fuera de los límites no se renderiza.
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		//Modo de polígonos:
		//	VK_POLYGON_MODE_FILL
		//	VK_POLYGON_MODE_LINE
		//	VK_POLYGON_MODE_POINT: vértice -> punto.
		rasterizer.polygonMode = polygonMode;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = cullMode;
		rasterizer.frontFace = frontFaceType;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		rasterizerHasBeenSet = true;
	}

	void GraphicsPipeline::SetMSAA(VkBool32 use, VkSampleCountFlagBits samples) {
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = use;
		multisampling.rasterizationSamples = samples;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		msaaHasBeenSet = true;
	}

	void GraphicsPipeline::SetDepthStencil(bool use) {
		depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		if (use) {
			depthStencilCreateInfo.depthTestEnable = VK_TRUE;
			depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
		}
		else {
			depthStencilCreateInfo.depthTestEnable = VK_FALSE;
			depthStencilCreateInfo.depthWriteEnable = VK_FALSE;
		}
		depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilCreateInfo.minDepthBounds = 0.0f;
		depthStencilCreateInfo.maxDepthBounds = 1.0f;
		depthStencilCreateInfo.stencilTestEnable = VK_FALSE;
		depthStencilCreateInfo.front = {};
		depthStencilCreateInfo.back = {};

		depthStencilHasBeenSet = true;
	}

	void GraphicsPipeline::SetPushConstants(VkShaderStageFlagBits shaderStage, uint32_t size, uint32_t offset) {
		VkPushConstantRange pushConstRange{};
		pushConstRange.stageFlags = shaderStage;
		pushConstRange.offset = offset;
		pushConstRange.size = size;

		pushConstRanges.push_back(pushConstRange);

		pushConstantsHaveBeenSet = true;
	}

	void GraphicsPipeline::SetLayout(VkDescriptorSetLayout* layout) {
		this->descriptorSetLayout = layout;
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = this->descriptorSetLayout;
		if (pushConstantsHaveBeenSet) {
			pipelineLayoutInfo.pushConstantRangeCount = pushConstRanges.size();
			pipelineLayoutInfo.pPushConstantRanges = pushConstRanges.data();
		}
		else {
			pipelineLayoutInfo.pushConstantRangeCount = 0;
			pipelineLayoutInfo.pPushConstantRanges = nullptr;
		}

		layoutHasBeenSet = true;
	}

	OskResult GraphicsPipeline::Create(VULKAN::Renderpass* renderpass) {
		OskResult res = loadShaders();
		if (res != OskResult::SUCCESS) {
			OSK_SHOW_TRACE();
			return res;
		}

		targetRenderpass = renderpass;
		
		if (!viewportHasBeenSet) {
			OSK_SHOW_ERROR("no se ha establecido el viewport.");
			
			return OskResult::ERROR_PIPELINE_VIEWPORT_NOT_SET;
		}
		if (!layoutHasBeenSet) {
			OSK_SHOW_ERROR("no se ha establecido el layout.");
		
			return OskResult::ERROR_PIPELINE_LAYOUT_NOT_SET;
		}

		if (!rasterizerHasBeenSet) {
			OSK_SHOW_WARNING("no se ha establecido el rasterizador. Se establecerá el rasterizador por defecto.");
			SetRasterizer(VK_TRUE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
		}
		
		if (!msaaHasBeenSet) {
			OSK_SHOW_WARNING("no se ha establecido MSAA. MSAA estará desactivado.");
			SetMSAA(false, VK_SAMPLE_COUNT_1_BIT);
		}

		if (!depthStencilHasBeenSet) {
			OSK_SHOW_WARNING("no se ha establecido el depth/stencil. Estará desactivado.");
			SetDepthStencil(false);
		}

		VkResult result = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &VulkanPipelineLayout);
		if (result != VK_SUCCESS) {
			OSK_SHOW_ERROR("crear pipelielayout.");

			return OskResult::ERROR_CREATE_PIPELINE_LAYOUT;
		}

		VkPipelineDynamicStateCreateInfo dynamicCreateInfo{};
		dynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		VkDynamicState states[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		dynamicCreateInfo.pDynamicStates = states;
		dynamicCreateInfo.dynamicStateCount = 2;
		dynamicCreateInfo.flags = 0;

		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencilCreateInfo;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicCreateInfo;
		pipelineInfo.layout = VulkanPipelineLayout;
		pipelineInfo.renderPass = targetRenderpass->VulkanRenderpass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = nullptr;
		pipelineInfo.basePipelineIndex = -1;

		result = vkCreateGraphicsPipelines(logicalDevice, nullptr, 1, &pipelineInfo, nullptr, &VulkanPipeline);
		if (result != VK_SUCCESS) {
			OSK_SHOW_ERROR("crear pipeline.");
		
			return OskResult::ERROR_CREATE_PIPELINE;
		}
	}

	void GraphicsPipeline::Bind(VkCommandBuffer commandBuffer) const {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanPipeline);
	}

	OskResult GraphicsPipeline::loadShaders() {
		//Código de los shaders.
		const std::vector<char> vertexCode = FileIO::ReadBinaryFromFile(vertexPath);
		const std::vector<char> fragmentCode = FileIO::ReadBinaryFromFile(fragmentPath);

		try {
			vertexShaderModule = createShaderModule(vertexCode);
			fragmentShaderModule = createShaderModule(fragmentCode);
		}
		catch (std::exception& e) {
			OSK_SHOW_ERROR("cargar shaders.");

			return OskResult::ERROR_LOAD_SHADERS;
		}

		//Crear los shaders.
		VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
		vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderStageInfo.module = vertexShaderModule;
		vertexShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
		fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragmentShaderStageInfo.module = fragmentShaderModule;
		fragmentShaderStageInfo.pName = "main";

		shaderStages[0] = vertexShaderStageInfo;
		shaderStages[1] = fragmentShaderStageInfo;

		//Información que le pasamos a los shaders.
		//Vértices.
		vertexBindingDesc = Vertex::GetBindingDescription();
		vertexInputDesc = Vertex::GetAttributeDescriptions_FullVertex();

		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDesc;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputDesc.size());
		vertexInputInfo.pVertexAttributeDescriptions = vertexInputDesc.data();

		return OskResult::SUCCESS;
	}

	OskResult GraphicsPipeline::ReloadShaders() {
		clearShaders();
		OskResult result = loadShaders();
		if (result != OskResult::SUCCESS) {
			Logger::Log(LogMessageLevels::NO, OSK_GET_TRACE);
			return result;
		}
		result = Create(targetRenderpass);
		if (result != OskResult::SUCCESS)
			Logger::Log(LogMessageLevels::NO, OSK_GET_TRACE);

		return result;
	}

	void GraphicsPipeline::clearShaders() {
		if (vertexShaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(logicalDevice, vertexShaderModule, nullptr);
			vertexShaderModule = VK_NULL_HANDLE;
		}
		if (fragmentShaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(logicalDevice, fragmentShaderModule, nullptr);
			fragmentShaderModule = VK_NULL_HANDLE;
		}
	}

	VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char>& code) const {
		VkShaderModule output;

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		
		VkResult result = vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &output);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear shader module.");

		return output;
	}

}