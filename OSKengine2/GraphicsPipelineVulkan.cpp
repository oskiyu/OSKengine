#include "GraphicsPipelineVulkan.h"

#include "DynamicArray.hpp"
#include "FileIO.h"
#include "GpuVulkan.h"
#include "PipelineCreateInfo.h"
#include "RenderpassVulkan.h"
#include "VertexVulkan.h"
#include "PipelineLayoutVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

VkPipelineColorBlendAttachmentState GetColorBlendInfo(const PipelineCreateInfo& info) {
	VkPipelineColorBlendAttachmentState output{};

	output.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	output.blendEnable = VK_TRUE;
	output.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	output.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	output.colorBlendOp = VK_BLEND_OP_ADD;
	output.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	output.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	output.alphaBlendOp = VK_BLEND_OP_ADD;
	
	return output;
}

VkPolygonMode GetPolygonMode(PolygonMode mode) {
	switch (mode) {
	case PolygonMode::FILL:
		return VK_POLYGON_MODE_FILL;
	case PolygonMode::LINE:
		return VK_POLYGON_MODE_LINE;
	default:
		return VK_POLYGON_MODE_FILL;
	}
}

VkCullModeFlagBits GetCullMode(PolygonCullMode mode) {
	switch (mode) {
	case PolygonCullMode::FRONT:
		return VK_CULL_MODE_FRONT_BIT;
	case PolygonCullMode::BACK:
		return VK_CULL_MODE_BACK_BIT;
	case PolygonCullMode::NONE:
		return VK_CULL_MODE_NONE;
	default:
		return VK_CULL_MODE_FRONT_AND_BACK;
	}
}

VkFrontFace GetFrontFaceMode(PolygonFrontFaceType type) {
	switch (type) {
	case PolygonFrontFaceType::CLOCKWISE:
		return VK_FRONT_FACE_CLOCKWISE;
	case PolygonFrontFaceType::COUNTERCLOCKWISE:
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	default:
		return VK_FRONT_FACE_CLOCKWISE;
	}
}

VkPipelineRasterizationStateCreateInfo GetResterizerInfo(const PipelineCreateInfo& info) {
	VkPipelineRasterizationStateCreateInfo output{};

	output.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	output.depthClampEnable = VK_FALSE; //FALSE: si el objeto está fuera de los límites no se renderiza.
	output.rasterizerDiscardEnable = VK_FALSE;
	//Modo de polígonos:
	//	VK_POLYGON_MODE_FILL
	//	VK_POLYGON_MODE_LINE
	//	VK_POLYGON_MODE_POINT: vértice -> punto.
	output.polygonMode = GetPolygonMode(info.polygonMode);
	output.lineWidth = 1.0f;
	output.cullMode = GetCullMode(info.cullMode);
	output.frontFace = GetFrontFaceMode(info.frontFaceType);
	output.depthBiasEnable = VK_FALSE;
	output.depthBiasConstantFactor = 0.0f;
	output.depthBiasClamp = 0.0f;
	output.depthBiasSlopeFactor = 0.0f;

	return output;
}

VkPipelineDepthStencilStateCreateInfo GetDepthInfo(const PipelineCreateInfo& info) {
	VkPipelineDepthStencilStateCreateInfo output{};

	output.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	output.depthTestEnable = VK_TRUE;
	output.depthWriteEnable = VK_TRUE;
	output.depthCompareOp = VK_COMPARE_OP_LESS;
	output.depthBoundsTestEnable = VK_FALSE;
	output.minDepthBounds = 0.0f;
	output.maxDepthBounds = 1.0f;
	output.stencilTestEnable = VK_FALSE;
	output.front = {};
	output.back = {};

	return output;
}

VkPipelineMultisampleStateCreateInfo GetMsaaInfo(const PipelineCreateInfo& info, const GpuVulkan& gpu) {
	VkPipelineMultisampleStateCreateInfo output{};

	output.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	output.sampleShadingEnable = VK_FALSE;
	output.rasterizationSamples = (VkSampleCountFlagBits)gpu.GetInfo().maxMsaaSamples;
	output.minSampleShading = 1.0f;
	output.pSampleMask = nullptr;
	output.alphaToCoverageEnable = VK_FALSE;
	output.alphaToOneEnable = VK_FALSE;

	return output;
}


GraphicsPipelineVulkan::GraphicsPipelineVulkan(RenderpassVulkan* renderpass)
	: targetRenderpass(renderpass) {

}

GraphicsPipelineVulkan::~GraphicsPipelineVulkan() {
	for (TSize i = 0; i < shaderModulesToDelete.GetSize(); i++)
		vkDestroyShaderModule(gpu->As<GpuVulkan>()->GetLogicalDevice(),
			shaderModulesToDelete.At(i), nullptr);

	vkDestroyPipeline(gpu->As<GpuVulkan>()->GetLogicalDevice(),
		pipeline, nullptr);
}

void GraphicsPipelineVulkan::Create(const MaterialLayout* materialLayout, IGpu* device, const PipelineCreateInfo& info) {
	gpu = device;

	layout = new PipelineLayoutVulkan(materialLayout);

	LoadVertexShader(info.vertexPath);
	LoadFragmentShader(info.fragmentPath);

	// Información básica del input del pipeline.
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	// Vertex
	VkVertexInputBindingDescription vertexBindingDesc = GetBindingDescription_Vertex3D();
	DynamicArray<VkVertexInputAttributeDescription> vertexAttribDescription = GetAttributeDescription_Vertex3D();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDesc;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttribDescription.GetSize());
	vertexInputInfo.pVertexAttributeDescriptions = vertexAttribDescription.GetData();

	// Color blending.
	VkPipelineColorBlendAttachmentState colorBlendAttachment = GetColorBlendInfo(info);
	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendCreateInfo.attachmentCount = 1;
	colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
	colorBlendCreateInfo.blendConstants[0] = 0.0f;
	colorBlendCreateInfo.blendConstants[1] = 0.0f;
	colorBlendCreateInfo.blendConstants[2] = 0.0f;
	colorBlendCreateInfo.blendConstants[3] = 0.0f;

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizerInfo = GetResterizerInfo(info);
	
	// Depth-stencil.
	VkPipelineDepthStencilStateCreateInfo depthInfo = GetDepthInfo(info);

	VkPipelineMultisampleStateCreateInfo msaaCreateInfo = GetMsaaInfo(info, *gpu->As<GpuVulkan>());

	// Estructuras dinámicas
	VkDynamicState states[] = { 
		VK_DYNAMIC_STATE_VIEWPORT, 
		VK_DYNAMIC_STATE_SCISSOR 
	};

	VkPipelineDynamicStateCreateInfo dynamicCreateInfo{};
	dynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicCreateInfo.pDynamicStates = states;
	dynamicCreateInfo.dynamicStateCount = _countof(states);
	dynamicCreateInfo.flags = 0;

	// Viewport (será dinámico)
	VkViewport viewport{};
	VkRect2D scissor{};

	VkPipelineViewportStateCreateInfo viewportInfo{};
	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = &viewport;
	viewportInfo.scissorCount = 1;
	viewportInfo.pScissors = &scissor;

	// Pipeline
	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = shaderStagesInfo.GetSize();
	pipelineCreateInfo.pStages = shaderStagesInfo.GetData();
	pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineCreateInfo.pViewportState = &viewportInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizerInfo;
	pipelineCreateInfo.pMultisampleState = &msaaCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &depthInfo;
	pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
	pipelineCreateInfo.pDynamicState = &dynamicCreateInfo;
	pipelineCreateInfo.layout = layout->As<PipelineLayoutVulkan>()->GetLayout();
	pipelineCreateInfo.renderPass = targetRenderpass->GetRenderpass();
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = nullptr;
	pipelineCreateInfo.basePipelineIndex = -1;

	VkResult result = vkCreateGraphicsPipelines(gpu->As<GpuVulkan>()->GetLogicalDevice(),
		nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline);
	OSK_ASSERT(result == VK_SUCCESS, "Error al crear el pipeline.");
}

VkPipeline GraphicsPipelineVulkan::GetPipeline() const {
	return pipeline;
}

void GraphicsPipelineVulkan::LoadVertexShader(const std::string& path) {
	VkShaderModule shaderModule = VK_NULL_HANDLE;

	// Lee el código SPIR-V.
	const DynamicArray<char> vertexCode = IO::FileIO::ReadBinaryFromFile(path);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = vertexCode.GetSize();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(vertexCode.GetData());

	VkResult result = vkCreateShaderModule(gpu->As<GpuVulkan>()->GetLogicalDevice(), 
		&createInfo, nullptr, &shaderModule);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido cargar el vertex shader.");

	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
	vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageInfo.module = shaderModule;
	vertexShaderStageInfo.pName = "main";

	shaderStagesInfo.Insert(vertexShaderStageInfo);
	shaderModulesToDelete.Insert(shaderModule);
}

void GraphicsPipelineVulkan::LoadFragmentShader(const std::string& path) {
	VkShaderModule shaderModule = VK_NULL_HANDLE;

	// Lee el código SPIR-V.
	const DynamicArray<char> fragmentCode = IO::FileIO::ReadBinaryFromFile(path);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = fragmentCode.GetSize();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(fragmentCode.GetData());

	VkResult result = vkCreateShaderModule(gpu->As<GpuVulkan>()->GetLogicalDevice(),
		&createInfo, nullptr, &shaderModule);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido cargar el vertex shader.");

	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
	fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageInfo.module = shaderModule;
	fragmentShaderStageInfo.pName = "main";

	shaderStagesInfo.Insert(fragmentShaderStageInfo);
	shaderModulesToDelete.Insert(shaderModule);
}
