#include "GraphicsPipelineVulkan.h"

#include "DynamicArray.hpp"
#include "FileIO.h"
#include "GpuVulkan.h"
#include "PipelineCreateInfo.h"
#include "RenderpassVulkan.h"
#include "PipelineLayoutVulkan.h"
#include "VertexInfo.h"
#include "Format.h"
#include "FormatVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


GraphicsPipelineVulkan::GraphicsPipelineVulkan() {

}

void GraphicsPipelineVulkan::Create(const MaterialLayout* materialLayout, IGpu* device, const PipelineCreateInfo& info, Format format, const VertexInfo& vertexInfo) {
	gpu = device;
	this->targetImageFormat = format;

	layout = new PipelineLayoutVulkan(materialLayout);

	LoadVertexShader(info.vertexPath);
	LoadFragmentShader(info.fragmentPath);

	if (info.tesselationControlPath != "") {
		LoadTesselationControlShader(info.tesselationControlPath);
		LoadTesselationEvaluationShader(info.tesselationEvaluationPath);
	}

	// Información básica del input del pipeline.
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	if (info.tesselationControlPath == "")
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	else
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	// Vertex
	VkVertexInputBindingDescription vertexBindingDesc = GetBindingDescription(vertexInfo);
	DynamicArray<VkVertexInputAttributeDescription> vertexAttribDescription = GetAttributeDescription(vertexInfo);

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

	VkPipelineTessellationStateCreateInfo tesselationInfo = GetTesselationInfo(info);

	// Estructuras dinámicas
	const VkDynamicState states[] = { 
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

	// Renderpass
	const VkFormat colorFormat = GetFormatVulkan(format);
	VkPipelineRenderingCreateInfoKHR renderingCreateInfo{};
	renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
	renderingCreateInfo.colorAttachmentCount = 1;
	renderingCreateInfo.pColorAttachmentFormats = &colorFormat;
	renderingCreateInfo.depthAttachmentFormat = GetFormatVulkan(Format::D32S8_SFLOAT_SUINT);
	renderingCreateInfo.stencilAttachmentFormat = GetFormatVulkan(Format::D32S8_SFLOAT_SUINT);

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
	if (info.tesselationControlPath == "")
		pipelineCreateInfo.pTessellationState = nullptr;
	else
		pipelineCreateInfo.pTessellationState = &tesselationInfo;
	pipelineCreateInfo.layout = layout->As<PipelineLayoutVulkan>()->GetLayout();
	pipelineCreateInfo.renderPass = VK_NULL_HANDLE;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = nullptr;
	pipelineCreateInfo.basePipelineIndex = -1;
	pipelineCreateInfo.pNext = &renderingCreateInfo;

	VkResult result = vkCreateGraphicsPipelines(gpu->As<GpuVulkan>()->GetLogicalDevice(),
		nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline);
	OSK_ASSERT(result == VK_SUCCESS, "Error al crear el pipeline.");
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

void GraphicsPipelineVulkan::LoadTesselationControlShader(const std::string& path) {
	VkShaderModule shaderModule = VK_NULL_HANDLE;

	// Lee el código SPIR-V.
	const DynamicArray<char> code = IO::FileIO::ReadBinaryFromFile(path);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.GetSize();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.GetData());

	VkResult result = vkCreateShaderModule(gpu->As<GpuVulkan>()->GetLogicalDevice(),
		&createInfo, nullptr, &shaderModule);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido cargar el tesselation control shader.");

	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	VkPipelineShaderStageCreateInfo shaderStageInfo{};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = "main";

	shaderStagesInfo.Insert(shaderStageInfo);
	shaderModulesToDelete.Insert(shaderModule);
}

void GraphicsPipelineVulkan::LoadTesselationEvaluationShader(const std::string& path) {
	VkShaderModule shaderModule = VK_NULL_HANDLE;

	// Lee el código SPIR-V.
	const DynamicArray<char> code = IO::FileIO::ReadBinaryFromFile(path);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.GetSize();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.GetData());

	VkResult result = vkCreateShaderModule(gpu->As<GpuVulkan>()->GetLogicalDevice(),
		&createInfo, nullptr, &shaderModule);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido cargar el tesselation evaluation shader.");

	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	VkPipelineShaderStageCreateInfo shaderStageInfo{};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = "main";

	shaderStagesInfo.Insert(shaderStageInfo);
	shaderModulesToDelete.Insert(shaderModule);
}

VkPipelineRasterizationStateCreateInfo GraphicsPipelineVulkan::GetResterizerInfo(const PipelineCreateInfo& info) const {
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

VkPipelineDepthStencilStateCreateInfo GraphicsPipelineVulkan::GetDepthInfo(const PipelineCreateInfo& info) const {
	VkPipelineDepthStencilStateCreateInfo output{};

	output.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	if (info.depthTestingType == DepthTestingType::NONE)
		output.depthTestEnable = VK_FALSE;
	else
		output.depthTestEnable = VK_TRUE;

	if (info.depthTestingType == DepthTestingType::READ_WRITE)
		output.depthWriteEnable = VK_TRUE;
	else
		output.depthWriteEnable = VK_FALSE;

	output.depthCompareOp = VK_COMPARE_OP_LESS;
	output.depthBoundsTestEnable = VK_FALSE;
	output.minDepthBounds = 0.0f;
	output.maxDepthBounds = 1.0f;
	output.stencilTestEnable = VK_FALSE;
	output.front = {};
	output.back = {};

	return output;
}

VkPipelineMultisampleStateCreateInfo GraphicsPipelineVulkan::GetMsaaInfo(const PipelineCreateInfo& info, const GpuVulkan& gpu) const {
	VkPipelineMultisampleStateCreateInfo output{};

	output.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	output.sampleShadingEnable = VK_FALSE;
	output.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;//(VkSampleCountFlagBits)gpu.GetInfo().maxMsaaSamples; TODO: change?
	output.minSampleShading = 1.0f;
	output.pSampleMask = nullptr;
	output.alphaToCoverageEnable = VK_FALSE;
	output.alphaToOneEnable = VK_FALSE;

	return output;
}

VkPipelineTessellationStateCreateInfo GraphicsPipelineVulkan::GetTesselationInfo(const PipelineCreateInfo& info) const {
	VkPipelineTessellationStateCreateInfo output{};

	output.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	output.pNext = NULL;
	output.flags = 0;
	output.patchControlPoints = 3;

	return output;
}
