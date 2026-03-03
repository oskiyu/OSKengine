#include "MaterialLoaderV1.h"

#include "MaterialType.h"
#include "Assert.h"
#include "PipelineCreateInfo.h"
#include "InvalidDescriptionFileException.h"
#include "FileIO.h"

#include "SpirvReflectionData.h"
#include "MaterialLayoutBuilder.h"
#include "UnreachableException.h"

#define OSK_LOAD_MATERIAL_INFO_COND(output, x) if (materialInfo.contains( x )) { const std::string str = materialInfo[ x ]; output = str; }

OSK::GRAPHICS::LoadedMaterialInfoV1 OSK::GRAPHICS::GetMaterialInfoV1(const nlohmann::json& materialInfo) {
	LoadedMaterialInfoV1 output{};

	output.name = materialInfo["name"];

	OSK_LOAD_MATERIAL_INFO_COND(output.vertexShaderPath,   "vertex_shader");
	OSK_LOAD_MATERIAL_INFO_COND(output.fragmentShaderPath, "fragment_shader");
	OSK_LOAD_MATERIAL_INFO_COND(output.computeShaderPath, "compute_shader");
	OSK_LOAD_MATERIAL_INFO_COND(output.tesselationControlShaderPath, "tesselation_control_shader");
	OSK_LOAD_MATERIAL_INFO_COND(output.tesselationEvaluationShaderPath, "tesselation_evaluation_shader");
	OSK_LOAD_MATERIAL_INFO_COND(output.meshAmplificationShaderPath, "amplification_shader");
	OSK_LOAD_MATERIAL_INFO_COND(output.meshShaderPath, "mesh_shader");
	OSK_LOAD_MATERIAL_INFO_COND(output.rayGenShaderPath, "rt_raygen_shader");
	OSK_LOAD_MATERIAL_INFO_COND(output.rayClosestHitShaderPath, "rt_closesthit_shader");
	OSK_LOAD_MATERIAL_INFO_COND(output.rayMissShaderPath, "rt_miss_shader");

	output.slotNames = GetSlotsNames(materialInfo);

	// Bind-less
	if (materialInfo.contains("config") && materialInfo["config"].contains("resource_vararray_max_count")) {
		output.config.usesUnspecifiedSizedArrays = true;
		const int maxCount = materialInfo["config"]["resource_vararray_max_count"];

		OSK_ASSERT(maxCount > 0, ASSETS::InvalidDescriptionFileException("config.resource_vararray_max_count debe ser mayor que 0.", ""));
		OSK_ASSERT(maxCount < 4096, ASSETS::InvalidDescriptionFileException("config.resource_vararray_max_count debe ser menor que 4096.", ""));

		output.config.resourceMaxCount = maxCount;
	}

	return output;
}

#undef OSK_LOAD_MATERIAL_INFO_COND

OSK::UniquePtr<OSK::GRAPHICS::MaterialLayout> OSK::GRAPHICS::LoadMaterialLayoutV1(const LoadedMaterialInfoV1& materialInfo, PipelineCreateInfo* info, MaterialType type) {
	
	// Bind-less
	info->usesUnspecifiedSizedArrays = materialInfo.config.usesUnspecifiedSizedArrays;
	info->maxUnspecifiedSizedArraysSize = materialInfo.config.resourceMaxCount;

	switch (type) {

	case MaterialType::GRAPHICS:
		return LoadGraphicsMaterialLayoutV1(materialInfo, info);

	case MaterialType::COMPUTE:
		return LoadComputeMaterialLayoutV1(materialInfo, info);

	case MaterialType::MESH:
		return LoadMeshMaterialLayoutV1(materialInfo, info);

	case MaterialType::RAYTRACING:
		return LoadRayTracingMaterialLayoutV1(materialInfo, info);

	default:
		throw UnreachableException("MaterialType no reconocido.");
		return OSK::UniquePtr<OSK::GRAPHICS::MaterialLayout>();
		break;
	}

	/*
	UIndex32 nextHlslBinding = 0;
	for (auto& [slotName, slot] : layout->GetAllSlots()) {
		for (auto& [_, binding] : slot.bindings) {
			binding.hlslDescriptorIndex = nextHlslBinding;
			nextHlslBinding++;
		}
	}

	for (auto& [_, pushConstant] : layout->GetAllPushConstants()) {
		pushConstant.hlslBindingIndex = nextHlslBinding;
		nextHlslBinding++;
	}*/
}

static OSK::UniquePtr<OSK::GRAPHICS::MaterialLayout> OSK::GRAPHICS::LoadGraphicsMaterialLayoutV1(const LoadedMaterialInfoV1& materialInfo, PipelineCreateInfo* info) {
	OSK_ASSERT(materialInfo.vertexShaderPath, ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'vertex_shader'.", ""));

	MaterialLayoutBuilder builder(materialInfo.slotNames, materialInfo.name);

	info->vertexPath = materialInfo.vertexShaderPath.value();
	info->fragmentPath = materialInfo.fragmentShaderPath ? materialInfo.fragmentShaderPath.value() : "";

	builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(info->vertexPath), ShaderStage::VERTEX));
	
	if (materialInfo.tesselationControlShaderPath) {
		builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(*materialInfo.tesselationControlShaderPath), ShaderStage::TESSELATION_CONTROL));
		info->tesselationControlPath = *materialInfo.tesselationControlShaderPath;
	}
	if (materialInfo.tesselationEvaluationShaderPath) {
		builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(*materialInfo.tesselationEvaluationShaderPath), ShaderStage::TESSELATION_EVALUATION));
		info->tesselationEvaluationPath = *materialInfo.tesselationEvaluationShaderPath;
	}

	if (materialInfo.fragmentShaderPath) {
		builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(*materialInfo.fragmentShaderPath), ShaderStage::FRAGMENT));
	}

	return builder.Build();
}

static OSK::UniquePtr<OSK::GRAPHICS::MaterialLayout> OSK::GRAPHICS::LoadComputeMaterialLayoutV1(const LoadedMaterialInfoV1& materialInfo, PipelineCreateInfo* info) {
	OSK_ASSERT(materialInfo.computeShaderPath, ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'compute_shader'.", ""));

	MaterialLayoutBuilder builder(materialInfo.slotNames, materialInfo.name);

	info->computeShaderPath = *materialInfo.computeShaderPath;

	builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(*materialInfo.computeShaderPath), ShaderStage::COMPUTE));
	
	return builder.Build();
}

static OSK::UniquePtr<OSK::GRAPHICS::MaterialLayout> OSK::GRAPHICS::LoadMeshMaterialLayoutV1(const LoadedMaterialInfoV1& materialInfo, PipelineCreateInfo* info) {
	OSK_ASSERT(materialInfo.meshShaderPath, ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'mesh_shader'.", ""));

	MaterialLayoutBuilder builder(materialInfo.slotNames, materialInfo.name);

	info->meshAmplificationShaderPath = materialInfo.meshAmplificationShaderPath
		? *materialInfo.meshAmplificationShaderPath
		: "";
	info->meshShaderPath = materialInfo.meshShaderPath
		? *materialInfo.meshShaderPath
		: "";
	info->fragmentPath = materialInfo.fragmentShaderPath
		? *materialInfo.fragmentShaderPath
		: "";

	if (materialInfo.meshAmplificationShaderPath) {
		builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(info->meshAmplificationShaderPath), ShaderStage::MESH_AMPLIFICATION));
	}

	if (materialInfo.meshShaderPath) {
		builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(info->meshShaderPath), ShaderStage::MESH));
	}

	if (materialInfo.fragmentShaderPath) {
		builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(info->fragmentPath), ShaderStage::FRAGMENT));
	}

	return builder.Build();
}

static OSK::UniquePtr<OSK::GRAPHICS::MaterialLayout> OSK::GRAPHICS::LoadRayTracingMaterialLayoutV1(const LoadedMaterialInfoV1& materialInfo, PipelineCreateInfo* info) {
	OSK_ASSERT(materialInfo.rayGenShaderPath, ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'rt_raygen_shader'.", ""));
	OSK_ASSERT(materialInfo.rayClosestHitShaderPath, ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'rt_closesthit_shader'.", ""));
	OSK_ASSERT(materialInfo.rayMissShaderPath, ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'rt_miss_shader'.", ""));

	info->rtRaygenShaderPath = *materialInfo.rayGenShaderPath;
	info->rtClosestHitShaderPath = *materialInfo.rayClosestHitShaderPath;
	info->rtMissShaderPath = *materialInfo.rayMissShaderPath;

	MaterialLayoutBuilder builder(materialInfo.slotNames, materialInfo.name);

	builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(*materialInfo.rayGenShaderPath), ShaderStage::RT_RAYGEN));
	builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(*materialInfo.rayClosestHitShaderPath), ShaderStage::RT_CLOSEST_HIT));
	builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(*materialInfo.rayMissShaderPath), ShaderStage::RT_MISS));

	return builder.Build();
}

static std::unordered_map<OSK::UIndex32, std::string> OSK::GRAPHICS::GetSlotsNames(const nlohmann::json& materialInfo) {
	std::unordered_map<OSK::UIndex32, std::string> output{};

	if (materialInfo.contains("slots")) {
		for (const auto& [index, name] : materialInfo["slots"].items()) {
			output[std::stoi(index)] = name;
		}
	}

	return output;
}
