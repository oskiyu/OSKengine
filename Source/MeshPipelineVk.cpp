#include "MeshPipelineVk.h"

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


MeshPipelineVk::MeshPipelineVk() {

}

void MeshPipelineVk::SetDebugName(const std::string& name) {
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
	nameInfo.objectHandle = (uint64_t)pipeline;
	nameInfo.pObjectName = name.c_str();
	nameInfo.pNext = nullptr;

	if (RendererVk::pvkSetDebugUtilsObjectNameEXT != nullptr)
		RendererVk::pvkSetDebugUtilsObjectNameEXT(m_gpu->GetLogicalDevice(), &nameInfo);
}

void MeshPipelineVk::Create(const MaterialLayout* materialLayout, IGpu* device, const PipelineCreateInfo& info) {
	m_gpu = device->As<GpuVk>();

	m_layout = new PipelineLayoutVk(materialLayout);

	if (info.meshAmplificationShaderPath != "") {
		LoadAmplificationShader(info.meshAmplificationShaderPath);
	}

	LoadMeshShader(info.meshShaderPath);

	if (info.fragmentPath != "") {
		LoadFragmentShader(info.fragmentPath);
	}

	// Información básica del input del pipeline.
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	if (info.polygonMode == PolygonMode::LINE)
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	else if (info.polygonMode == PolygonMode::POINT)
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	else
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;


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

	VkPipelineMultisampleStateCreateInfo msaaCreateInfo = GetMsaaInfo(info, *m_gpu);

	VkPipelineTessellationStateCreateInfo tesselationInfo = GetTesselationInfo(info);

	// Estructuras dinámicas
	const std::array<const VkDynamicState, 2> states = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicCreateInfo{};
	dynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicCreateInfo.pDynamicStates = states.data();
	dynamicCreateInfo.dynamicStateCount = states.size();
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

	pipelineCreateInfo.pVertexInputState = nullptr;
	pipelineCreateInfo.pInputAssemblyState = nullptr;
	pipelineCreateInfo.pTessellationState = nullptr;

	pipelineCreateInfo.pViewportState = &viewportInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizerInfo;
	pipelineCreateInfo.pMultisampleState = &msaaCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &depthInfo;
	pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
	pipelineCreateInfo.pDynamicState = &dynamicCreateInfo;
	pipelineCreateInfo.layout = m_layout->As<PipelineLayoutVk>()->GetLayout();
	pipelineCreateInfo.renderPass = VK_NULL_HANDLE;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = nullptr;
	pipelineCreateInfo.basePipelineIndex = -1;
	pipelineCreateInfo.pNext = &renderingCreateInfo;

	VkResult result = vkCreateGraphicsPipelines(m_gpu->GetLogicalDevice(),
		nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline);

	OSK_ASSERT(result == VK_SUCCESS, PipelineCreationException(result));
}


void MeshPipelineVk::LoadAmplificationShader(const std::string& path) {
	// Lee el código SPIR-V.
	const DynamicArray<char> fragmentCode = IO::FileIO::ReadBinaryFromFile(path);
	const VkShaderModule shaderModule = CreateShaderModule(
		fragmentCode.GetFullSpan(),
		path,
		m_gpu->GetLogicalDevice());

	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
	fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageInfo.stage = VK_SHADER_STAGE_TASK_BIT_EXT;
	fragmentShaderStageInfo.module = shaderModule;
	fragmentShaderStageInfo.pName = "main";

	shaderStagesInfo.Insert(fragmentShaderStageInfo);
	shaderModulesToDelete.Insert(shaderModule);
}

void MeshPipelineVk::LoadMeshShader(const std::string& path) {
	// Lee el código SPIR-V.
	const DynamicArray<char> fragmentCode = IO::FileIO::ReadBinaryFromFile(path);
	const VkShaderModule shaderModule = CreateShaderModule(
		fragmentCode.GetFullSpan(),
		path,
		m_gpu->GetLogicalDevice());

	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
	fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageInfo.stage = VK_SHADER_STAGE_MESH_BIT_EXT;
	fragmentShaderStageInfo.module = shaderModule;
	fragmentShaderStageInfo.pName = "main";

	shaderStagesInfo.Insert(fragmentShaderStageInfo);
	shaderModulesToDelete.Insert(shaderModule);
}

void MeshPipelineVk::LoadFragmentShader(const std::string& path) {
	// Lee el código SPIR-V.
	const DynamicArray<char> fragmentCode = IO::FileIO::ReadBinaryFromFile(path);
	const VkShaderModule shaderModule = CreateShaderModule(
		fragmentCode.GetFullSpan(),
		path,
		m_gpu->GetLogicalDevice());

	// Insertar el stage en el array, para poder insertarlo al crear el pipeline.
	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
	fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageInfo.module = shaderModule;
	fragmentShaderStageInfo.pName = "main";

	shaderStagesInfo.Insert(fragmentShaderStageInfo);
	shaderModulesToDelete.Insert(shaderModule);
}

#endif
