#include "MaterialSystem.h"

#include "FileIO.h"
#include "MaterialLayout.h"

#include "Material.h"
#include "PipelineCreateInfo.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "RenderApiType.h"

#include "MaterialExceptions.h"
#include "IRenderer.h"

#include "InvalidDescriptionFileException.h"
#include "RendererExceptions.h"

#include "MaterialLoaderV1.h"

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::GRAPHICS;

ShaderBindingType GetShaderBindingType(std::string_view type) {
	if (type == "UNIFORM")
		return ShaderBindingType::UNIFORM_BUFFER;

	if (type == "CUBEMAP")
		return ShaderBindingType::CUBEMAP;

	return ShaderBindingType::TEXTURE;
}

ShaderStage GetShaderStage(std::string_view type) {
	if (type == "VERTEX")
		return ShaderStage::VERTEX;

	if (type == "FRAGMENT")
		return ShaderStage::FRAGMENT;

	if (type == "TESSELATION")
		return ShaderStage::TESSELATION_ALL;

	OSK_ASSERT(false, NotImplementedException());

	return ShaderStage::FRAGMENT;
}

PolygonMode GetPolygonMode(std::string_view type) {
	if (type == "TRIANGLE_WIDEFRAME")
		return PolygonMode::TRIANGLE_WIDEFRAME;
	else if (type == "LINE")
		return PolygonMode::LINE;
	else if (type == "POINT")
		return PolygonMode::POINT;
	else
		return PolygonMode::TRIANGLE_FILL;
}


MaterialSystem::~MaterialSystem() {

}

void MaterialSystem::LoadMaterialV0(MaterialLayout* layout, const nlohmann::json& materialInfo, PipelineCreateInfo* info) {
	OSK_ASSERT(materialInfo.contains("shader_file"), ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'shader_file'.", ""));
	OSK_ASSERT(materialInfo.contains("layout"), ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'layout'.", ""));

	info->precompiledHlslShaders = true;

	// Material file
	std::string materialName = materialInfo["name"];
	std::string shaderFilePath = materialInfo["shader_file"];

	for (auto& slotInfo : materialInfo["layout"]["slots"]) {
		MaterialLayoutSlot slot{};
		slot.name = slotInfo["name"];

		for (auto& bindingInfo : slotInfo["bindings"]) {
			MaterialLayoutBinding binding{};

			binding.name = bindingInfo["name"];
			binding.type = GetShaderBindingType(bindingInfo["type"]);

			slot.bindings[binding.name] = binding;
		}

		layout->AddSlot(slot);
	}

	for (auto& pushConstantInfo : materialInfo["layout"]["push_constants"]) {
		MaterialLayoutPushConstant pushConst{};

		pushConst.name = pushConstantInfo["name"];
		pushConst.size = pushConstantInfo["size"];

		layout->AddPushConstant(pushConst);
	}

	// Shader file.
	{
		nlohmann::json shaderInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(shaderFilePath));

		int fileVersion = shaderInfo["spec_ver"];

		if (Engine::GetRenderer()->GetRenderApi() == RenderApiType::VULKAN) {
			info->vertexPath = shaderInfo["glsl_shaders"]["vertex"];
			info->fragmentPath = shaderInfo["glsl_shaders"]["fragment"];

			if (shaderInfo["glsl_shaders"].contains("tesselation_control"))
				info->tesselationControlPath = shaderInfo["glsl_shaders"]["tesselation_control"];

			if (shaderInfo["glsl_shaders"].contains("tesselation_evaluation"))
				info->tesselationEvaluationPath = shaderInfo["glsl_shaders"]["tesselation_evaluation"];
		}
		else if (Engine::GetRenderer()->GetRenderApi() == RenderApiType::DX12) {
			info->vertexPath = shaderInfo["hlsl_shaders"]["vertex"];
			info->fragmentPath = shaderInfo["hlsl_shaders"]["fragment"];

			if (shaderInfo["hlsl_shaders"].contains("tesselation_control"))
				info->tesselationControlPath = shaderInfo["hlsl_shaders"]["tesselation_control"];

			if (shaderInfo["hlsl_shaders"].contains("tesselation_evaluation"))
				info->tesselationEvaluationPath = shaderInfo["hlsl_shaders"]["tesselation_evaluation"];
		}

		for (auto& slotInfo : shaderInfo["slots"]) {
			std::string slotName = slotInfo["name"];
			auto& slot = layout->GetSlot(slotName);
			slot.glslSetIndex = slotInfo["glsl_set"];

			for (auto& stage : slotInfo["shader_stages"])
				EFTraits::AddFlag(&slot.stage, GetShaderStage(stage));

			for (auto& bindingInfo : slotInfo["bindings"]) {
				std::string bindingName = bindingInfo["name"];

				slot.bindings[bindingName].glslIndex = bindingInfo["glsl_index"];
				slot.bindings[bindingName].hlslIndex = bindingInfo["hlsl_index"];
			}
		}

		for (auto& pushConstantInfo : shaderInfo["push_constants"]) {
			auto& pushConst = layout->GetPushConstant(pushConstantInfo["name"]);

			for (auto& stage : pushConstantInfo["shader_stages"])
				EFTraits::AddFlag(&pushConst.stage, GetShaderStage(stage));

			pushConst.hlslIndex = pushConstantInfo["hlsl_index"];
		}
	}

	UIndex32 hlslDescIndex = 0;
	for (auto& [name, slot] : layout->GetAllSlots()) {
		for (auto& binding : slot.bindings) {
			binding.second.hlslDescriptorIndex = hlslDescIndex;

			hlslDescIndex++;
		}
	}

	for (auto& pushConstInfo : layout->GetAllPushConstants()) {
		pushConstInfo.second.hlslBindingIndex = hlslDescIndex;
		hlslDescIndex++;
	}

}

Material* MaterialSystem::LoadMaterial(const std::string& path) {
	if (materialsPathTable.contains(path))
		return materialsPathTable.at(path);

	UniquePtr<MaterialLayout> layout = MakeUnique<MaterialLayout>(path);

	// Material file.
	nlohmann::json materialInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(path));

	OSK_ASSERT(materialInfo.contains("file_type"), ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'file_type'.", ""));
	OSK_ASSERT(materialInfo.contains("name"), ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'name'.", ""));

	MaterialType type = MaterialType::GRAPHICS;
	if (materialInfo["file_type"] == "MATERIAL_RT") {
		OSK_ASSERT(Engine::GetRenderer()->SupportsRaytracing(), RayTracingNotSupportedException());
		type = MaterialType::RAYTRACING;
	}
	else if (materialInfo["file_type"] == "MATERIAL_COMPUTE") {
		type = MaterialType::COMPUTE;
	}
	else if (materialInfo["file_type"] == "MATERIAL_MESH") {
		type = MaterialType::MESH;
	}
	
	VertexInfo vertexType = {};
	
	if (type == MaterialType::GRAPHICS) {
		OSK_ASSERT(materialInfo.contains("vertex_type"), ASSETS::InvalidDescriptionFileException("Archivo de material incorrecto: no se encuentra 'vertex_type'.", path));
		vertexType = vertexTypesTable.at(materialInfo["vertex_type"]);
	}

	int fileVersion = materialInfo["spec_ver"];

	PipelineCreateInfo info{};
	info.cullMode = PolygonCullMode::BACK;
	info.frontFaceType = PolygonFrontFaceType::COUNTERCLOCKWISE;

	if (fileVersion == 0)
		LoadMaterialV0(layout.GetPointer(), materialInfo, &info);
	else if (fileVersion == 1)
		layout = LoadMaterialLayoutV1(materialInfo, &info, type);
	else
		OSK_ASSERT(false, ASSETS::InvalidDescriptionFileException("La versión del archivo de material no está soportada.", path))

	if (materialInfo.find("config") != materialInfo.end()) {
		if (materialInfo["config"].contains("depth_testing")) {
			if (materialInfo["config"]["depth_testing"] == "none")
				info.depthTestingType = DepthTestingType::NONE;
			else if (materialInfo["config"]["depth_testing"] == "read")
				info.depthTestingType = DepthTestingType::READ;
			else if (materialInfo["config"]["depth_testing"] == "read/write")
				info.depthTestingType = DepthTestingType::READ_WRITE;
			else
				OSK_ASSERT(false, ASSETS::InvalidDescriptionFileException("Error en el archivo de material: config depth_testing inválido.", path));
		}

		if (materialInfo["config"].contains("cull_mode")) {
			if (materialInfo["config"]["cull_mode"] == "none")
				info.cullMode = PolygonCullMode::NONE;
			else if (materialInfo["config"]["cull_mode"] == "front")
				info.cullMode = PolygonCullMode::FRONT;
			else if (materialInfo["config"]["cull_mode"] == "back")
				info.cullMode = PolygonCullMode::BACK;
			else
				OSK_ASSERT(false, ASSETS::InvalidDescriptionFileException("Error en el archivo de material: config cull_mode inválido.", path));
		}

		if (materialInfo["config"].contains("disable_alpha_blending"))
			info.useAlphaBlending = false;
	}

	if (materialInfo.contains("polygon_mode"))
		info.polygonMode = GetPolygonMode(materialInfo["polygon_mode"]);

	// Engine::GetLogger()->DebugLog(ToString(*layout));

	auto material = MakeUnique<Material>(info, std::move(layout), vertexType, type);
	auto* output = material.GetPointer();

	material->SetName(materialInfo["name"]);

	materials.Insert(std::move(material));

	materialsPathTable[path] = output;
	materialsNameTable[materialInfo["name"]] = output;

	return output;
}

Material* MaterialSystem::GetMaterialByName(const std::string& name) const {
	OSK_ASSERT(materialsNameTable.contains(name), MaterialNotFoundException(name));
	return materialsNameTable.at(name);
}

void MaterialSystem::ReloadMaterialByPath(const std::string& path) {
	OSK_ASSERT(materialsPathTable.contains(path), MaterialNotFoundException(path));
	Engine::GetRenderer()->WaitForCompletion();
	LoadMaterial(path)->_Reload();
}

void MaterialSystem::ReloadMaterialByName(const std::string& name) {
	OSK_ASSERT(materialsNameTable.contains(name), MaterialNotFoundException(name));
	Engine::GetRenderer()->WaitForCompletion();
	GetMaterialByName(name)->_Reload();
}

void MaterialSystem::ReloadAllMaterials() {
	Engine::GetRenderer()->WaitForCompletion();
	for (auto& m : materials)
		m->_Reload();
}
