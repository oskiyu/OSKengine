#include "MaterialLoaderV1.h"

#include "MaterialType.h"
#include "Assert.h"
#include "PipelineCreateInfo.h"
#include "InvalidDescriptionFileException.h"
#include "FileIO.h"

#include "SpirvReflectionData.h"
#include "MaterialLayoutBuilder.h"
#include "UnreachableException.h"

OSK::UniquePtr<OSK::GRAPHICS::MaterialLayout> OSK::GRAPHICS::LoadMaterialLayoutV1(const nlohmann::json& materialInfo, PipelineCreateInfo* info, MaterialType type) {
	// Bind-less
	if (materialInfo.contains("config") && materialInfo["config"].contains("resource_vararray_max_count")) {
		info->usesUnspecifiedSizedArrays = true;
		const int maxCount = materialInfo["config"]["resource_vararray_max_count"];

		OSK_ASSERT(maxCount > 0, ASSETS::InvalidDescriptionFileException("config.resource_vararray_max_count debe ser mayor que 0.", ""));
		OSK_ASSERT(maxCount < 4096, ASSETS::InvalidDescriptionFileException("config.resource_vararray_max_count debe ser menor que 4096.", ""));

		info->maxUnspecifiedSizedArraysSize = maxCount;
	}

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

static OSK::UniquePtr<OSK::GRAPHICS::MaterialLayout> OSK::GRAPHICS::LoadGraphicsMaterialLayoutV1(const nlohmann::json& materialInfo, PipelineCreateInfo* info) {
	OSK_ASSERT(materialInfo.contains("vertex_shader"), ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'vertex_shader'.", ""));

	MaterialLayoutBuilder builder(GetSlotsNames(materialInfo), materialInfo["name"]);

	info->vertexPath = materialInfo["vertex_shader"];
	info->fragmentPath = materialInfo.contains("fragment_shader")
		? materialInfo["fragment_shader"]
		: "";

	builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(materialInfo["vertex_shader"]), ShaderStage::VERTEX));
	
	if (materialInfo.contains("tesselation_control_shader")) {
		builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(materialInfo["tesselation_control_shader"]), ShaderStage::TESSELATION_CONTROL));
		info->tesselationControlPath = materialInfo["tesselation_control_shader"];
	}
	if (materialInfo.contains("tesselation_evaluation_shader")) {
		builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(materialInfo["tesselation_evaluation_shader"]), ShaderStage::TESSELATION_EVALUATION));
		info->tesselationEvaluationPath = materialInfo["tesselation_evaluation_shader"];
	}

	if (materialInfo.contains("fragment_shader")) {
		builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(materialInfo["fragment_shader"]), ShaderStage::FRAGMENT));
	}

	return builder.Build();
}

static OSK::UniquePtr<OSK::GRAPHICS::MaterialLayout> OSK::GRAPHICS::LoadComputeMaterialLayoutV1(const nlohmann::json& materialInfo, PipelineCreateInfo* info) {
	OSK_ASSERT(materialInfo.contains("compute_shader"), ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'compute_shader'.", ""));

	MaterialLayoutBuilder builder(GetSlotsNames(materialInfo), materialInfo["name"]);

	info->computeShaderPath = materialInfo["compute_shader"];

	builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(materialInfo["compute_shader"]), ShaderStage::COMPUTE));
	
	return builder.Build();
}

static OSK::UniquePtr<OSK::GRAPHICS::MaterialLayout> OSK::GRAPHICS::LoadMeshMaterialLayoutV1(const nlohmann::json& materialInfo, PipelineCreateInfo* info) {
	OSK_ASSERT(materialInfo.contains("mesh_shader"), ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'mesh_shader'.", ""));

	MaterialLayoutBuilder builder(GetSlotsNames(materialInfo), materialInfo["name"]);

	info->meshAmplificationShaderPath = materialInfo.contains("amplification_shader")
		? materialInfo["amplification_shader"]
		: "";
	info->meshShaderPath = materialInfo.contains("mesh_shader")
		? materialInfo["mesh_shader"]
		: "";
	info->fragmentPath = materialInfo.contains("fragment_shader")
		? materialInfo["fragment_shader"]
		: "";

	if (materialInfo.contains("amplification_shader")) {
		builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(materialInfo["amplification_shader"]), ShaderStage::MESH_AMPLIFICATION));
	}

	if (materialInfo.contains("mesh_shader")) {
		builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(materialInfo["mesh_shader"]), ShaderStage::MESH));
	}

	if (materialInfo.contains("fragment_shader")) {
		builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(materialInfo["fragment_shader"]), ShaderStage::FRAGMENT));
	}

	return builder.Build();
}

static OSK::UniquePtr<OSK::GRAPHICS::MaterialLayout> OSK::GRAPHICS::LoadRayTracingMaterialLayoutV1(const nlohmann::json& materialInfo, PipelineCreateInfo* info) {
	OSK_ASSERT(materialInfo.contains("rt_raygen_shader"), ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'rt_raygen_shader'.", ""));
	OSK_ASSERT(materialInfo.contains("rt_closesthit_shader"), ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'rt_closesthit_shader'.", ""));
	OSK_ASSERT(materialInfo.contains("rt_miss_shader"), ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'rt_miss_shader'.", ""));

	info->rtRaygenShaderPath = materialInfo["rt_raygen_shader"];
	info->rtClosestHitShaderPath = materialInfo["rt_closesthit_shader"];
	info->rtMissShaderPath = materialInfo["rt_miss_shader"];

	MaterialLayoutBuilder builder(GetSlotsNames(materialInfo), materialInfo["name"]);

	builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(materialInfo["rt_raygen_shader"]), ShaderStage::RT_RAYGEN));
	builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(materialInfo["rt_closesthit_shader"]), ShaderStage::RT_CLOSEST_HIT));
	builder.Apply(SpirvReflectionData(IO::FileIO::ReadBinaryFromFile(materialInfo["rt_miss_shader"]), ShaderStage::RT_MISS));

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
