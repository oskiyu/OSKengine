#include "GraphicsPipelineVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "DynamicArray.hpp"
#include "FileIO.h"
#include "GpuVk.h"
#include "PipelineCreateInfo.h"
#include "PipelineLayoutVk.h"
#include "VertexInfo.h"
#include "Format.h"
#include "FormatVk.h"
#include "RendererVk.h"

#include "PipelinesExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


GraphicsPipelineVk::GraphicsPipelineVk() {

}

void GraphicsPipelineVk::SetDebugName(const std::string& name) {
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
	nameInfo.objectHandle = (uint64_t)pipeline;
	nameInfo.pObjectName = name.c_str();
	nameInfo.pNext = nullptr;

	if (RendererVk::pvkSetDebugUtilsObjectNameEXT != nullptr)
		RendererVk::pvkSetDebugUtilsObjectNameEXT(gpu->As<GpuVk>()->GetLogicalDevice(), &nameInfo);
}

void GraphicsPipelineVk::Create(const MaterialLayout* materialLayout, IGpu* device, const PipelineCreateInfo& info, const VertexInfo& vertexInfo) {
	gpu = device;

	layout = MakeUnique<PipelineLayoutVk>(materialLayout);

	LoadVertexShader(info.vertexPath);

	if (info.fragmentPath != "") {
		LoadFragmentShader(info.fragmentPath);
	}

	if (info.tesselationControlPath != "") {
		LoadTesselationControlShader(info.tesselationControlPath);
		LoadTesselationEvaluationShader(info.tesselationEvaluationPath);
	}

	// Informaci�n b�sica del input del pipeline.
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	if (info.tesselationControlPath == "") {
		if (info.polygonMode == PolygonMode::LINE)
			inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		else if (info.polygonMode == PolygonMode::POINT)
			inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		else
			inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}
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
	const VkPipelineColorBlendAttachmentState colorBlendAttachment = GetColorBlendInfo(info);

	DynamicArray<VkPipelineColorBlendAttachmentState> colorBlends;
	for (UIndex32 i = 0; i < info.formats.GetSize(); i++)
		colorBlends.Insert(colorBlendAttachment);

	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendCreateInfo.attachmentCount = static_cast<uint32_t>(colorBlends.GetSize());
	colorBlendCreateInfo.pAttachments = colorBlends.GetData();
	colorBlendCreateInfo.blendConstants[0] = 0.0f;
	colorBlendCreateInfo.blendConstants[1] = 0.0f;
	colorBlendCreateInfo.blendConstants[2] = 0.0f;
	colorBlendCreateInfo.blendConstants[3] = 0.0f;

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizerInfo = GetResterizerInfo(info);
	
	// Depth-stencil.
	VkPipelineDepthStencilStateCreateInfo depthInfo = GetDepthInfo(info);

	VkPipelineMultisampleStateCreateInfo msaaCreateInfo = GetMsaaInfo(info, *gpu->As<GpuVk>());

	VkPipelineTessellationStateCreateInfo tesselationInfo = GetTesselationInfo(info);

	// Estructuras din�micas
	const VkDynamicState states[] = { 
		VK_DYNAMIC_STATE_VIEWPORT, 
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicCreateInfo{};
	dynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicCreateInfo.pDynamicStates = states;
	dynamicCreateInfo.dynamicStateCount = _countof(states);
	dynamicCreateInfo.flags = 0;

	// Viewport (ser� din�mico)
	VkViewport viewport{};
	VkRect2D scissor{};

	VkPipelineViewportStateCreateInfo viewportInfo{};
	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = &viewport;
	viewportInfo.scissorCount = 1;
	viewportInfo.pScissors = &scissor;

	// Renderpass
	DynamicArray<VkFormat> colorFormats;
	for (const auto format : info.formats)
		colorFormats.Insert(GetFormatVk(format));

	VkPipelineRenderingCreateInfoKHR renderingCreateInfo{};
	renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
	renderingCreateInfo.colorAttachmentCount = static_cast<uint32_t>(colorFormats.GetSize());
	renderingCreateInfo.pColorAttachmentFormats = colorFormats.GetData();
	renderingCreateInfo.depthAttachmentFormat = GetFormatVk(info.depthFormat);
	if (FormatSupportsStencil(info.depthFormat))
		renderingCreateInfo.stencilAttachmentFormat = GetFormatVk(info.depthFormat);

	// Pipeline
	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStagesInfo.GetSize());
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
	pipelineCreateInfo.layout = layout->As<PipelineLayoutVk>()->GetLayout();
	pipelineCreateInfo.renderPass = VK_NULL_HANDLE;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = nullptr;
	pipelineCreateInfo.basePipelineIndex = -1;
	pipelineCreateInfo.pNext = &renderingCreateInfo;

	VkResult result = vkCreateGraphicsPipelines(gpu->As<GpuVk>()->GetLogicalDevice(),
		nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline);
	OSK_ASSERT(result == VK_SUCCESS, PipelineCreationException(result));
}

void GraphicsPipelineVk::LoadVertexShader(const std::string& path) {
	// Lee el c�digo SPIR-V.
	const DynamicArray<char> vertexCode = IO::FileIO::ReadBinaryFromFile(path);
	const VkShaderModule shaderModule = CreateShaderModule(
		vertexCode.GetFullSpan(),
		path,
		gpu->As<GpuVk>()->GetLogicalDevice());

	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
	vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageInfo.module = shaderModule;
	vertexShaderStageInfo.pName = "main";

	shaderStagesInfo.Insert(vertexShaderStageInfo);
	shaderModulesToDelete.Insert(shaderModule);
}

void GraphicsPipelineVk::LoadFragmentShader(const std::string& path) {
	// Lee el c�digo SPIR-V.
	const DynamicArray<char> fragmentCode = IO::FileIO::ReadBinaryFromFile(path);
	const VkShaderModule shaderModule = CreateShaderModule(
		fragmentCode.GetFullSpan(),
		path,
		gpu->As<GpuVk>()->GetLogicalDevice());

	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
	fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageInfo.module = shaderModule;
	fragmentShaderStageInfo.pName = "main";

	shaderStagesInfo.Insert(fragmentShaderStageInfo);
	shaderModulesToDelete.Insert(shaderModule);
}

void GraphicsPipelineVk::LoadTesselationControlShader(const std::string& path) {
	// Lee el c�digo SPIR-V.
	const DynamicArray<char> code = IO::FileIO::ReadBinaryFromFile(path);
	const VkShaderModule shaderModule = CreateShaderModule(
		code.GetFullSpan(),
		path,
		gpu->As<GpuVk>()->GetLogicalDevice());

	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	VkPipelineShaderStageCreateInfo shaderStageInfo{};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = "main";

	shaderStagesInfo.Insert(shaderStageInfo);
	shaderModulesToDelete.Insert(shaderModule);
}

void GraphicsPipelineVk::LoadTesselationEvaluationShader(const std::string& path) {
	// Lee el c�digo SPIR-V.
	const DynamicArray<char> code = IO::FileIO::ReadBinaryFromFile(path);
	const VkShaderModule shaderModule = CreateShaderModule(
		code.GetFullSpan(),
		path,
		gpu->As<GpuVk>()->GetLogicalDevice());


	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	VkPipelineShaderStageCreateInfo shaderStageInfo{};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = "main";

	shaderStagesInfo.Insert(shaderStageInfo);
	shaderModulesToDelete.Insert(shaderModule);
}

#endif
