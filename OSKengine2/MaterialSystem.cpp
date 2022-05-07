#include "MaterialSystem.h"

#include "FileIO.h"
#include "MaterialLayout.h"

#include <json.hpp>
#include "Material.h"
#include "PipelineCreateInfo.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "RenderApiType.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ShaderBindingType GetShaderBindingType(const std::string& type) {
	if (type == "UNIFORM")
		return ShaderBindingType::UNIFORM_BUFFER;

	if (type == "CUBEMAP")
		return ShaderBindingType::CUBEMAP;

	return ShaderBindingType::TEXTURE;
}

ShaderStage GetShaderStage(const std::string& type) {
	if (type == "VERTEX")
		return ShaderStage::VERTEX;

	if (type == "FRAGMENT")
		return ShaderStage::FRAGMENT;

	if (type == "TESSELATION")
		return ShaderStage::TESSELATION;

	OSK_ASSERT(false, type + " no es un shader stage válido.");

	return ShaderStage::FRAGMENT;
}

PolygonMode GetPolygonMode(const std::string& type) {
	if (type == "LINE")
		return PolygonMode::LINE;
	else
		return PolygonMode::FILL;
}


MaterialSystem::~MaterialSystem() {
	for (TSize i = 0; i < materials.GetSize(); i++)
		delete materials[i].GetPointer();
}

Material* MaterialSystem::LoadMaterial(const std::string& path) {
	MaterialLayout* layout = new MaterialLayout;

	std::string shaderFilePath;

	std::string vertexPath;
	std::string fragmentPath;
	std::string tesselationControlPath = "";
	std::string tesselationEvaluationPath = "";

	VertexInfo vertexType;
	PolygonMode polygonMode = PolygonMode::FILL;

	// Material file.
	nlohmann::json materialInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(path));
	{
		OSK_ASSERT(materialInfo.contains("file_type"), "Archivo de material incorrecto: no se encuentra 'file_type'.");
		OSK_ASSERT(materialInfo.contains("name"), "Archivo de material incorrecto: no se encuentra 'name'.");
		OSK_ASSERT(materialInfo.contains("shader_file"), "Archivo de material incorrecto: no se encuentra 'shader_file'.");
		OSK_ASSERT(materialInfo.contains("layout"), "Archivo de material incorrecto: no se encuentra 'layout'.");
		OSK_ASSERT(materialInfo.contains("vertex_type"), "Archivo de material incorrecto: no se encuentra 'vertex_type'.");

		OSK_ASSERT(materialInfo["file_type"] == "MATERIAL", std::string("Archivo ") + path + "no es un material.");
		
		int fileVersion = materialInfo["spec_ver"];
		std::string materialName = materialInfo["name"];
		shaderFilePath = materialInfo["shader_file"];

		for (auto& slotInfo : materialInfo["layout"]["slots"]) {
			MaterialLayoutSlot slot{};
			slot.name = slotInfo["name"];
			
			for (auto& bindingInfo : slotInfo["bindings"]) {
				MaterialLayoutBinding binding{};

				binding.name = bindingInfo["name"];
				binding.type = GetShaderBindingType(bindingInfo["type"]);

				slot.bindings.Insert(binding.name, binding);
			}

			layout->AddSlot(slot);
		}

		for (auto& pushConstantInfo : materialInfo["layout"]["push_constants"]) {
			MaterialLayoutPushConstant pushConst{};

			pushConst.name = pushConstantInfo["name"];
			pushConst.size = pushConstantInfo["size"];

			layout->AddPushConstant(pushConst);
		}

		vertexType = vertexTypesTable.Get(materialInfo["vertex_type"]);

		if (materialInfo.contains("polygon_mode"))
			polygonMode = GetPolygonMode(materialInfo["polygon_mode"]);
	}

	// Shader file.
	{
		nlohmann::json shaderInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(shaderFilePath));

		//OSK_ASSERT(shaderInfo["file_type"] == "SHADER", shaderFilePath + "no es un shader.");
		
		int fileVersion = shaderInfo["spec_ver"];

		if (Engine::GetRenderer()->GetRenderApi() == RenderApiType::OPENGL
			|| Engine::GetRenderer()->GetRenderApi() == RenderApiType::VULKAN) {
			vertexPath = shaderInfo["glsl_shaders"]["vertex"];
			fragmentPath = shaderInfo["glsl_shaders"]["fragment"];

			if (shaderInfo["glsl_shaders"].contains("tesselation_control"))
				tesselationControlPath = shaderInfo["glsl_shaders"]["tesselation_control"];

			if (shaderInfo["glsl_shaders"].contains("tesselation_evaluation"))
				tesselationEvaluationPath = shaderInfo["glsl_shaders"]["tesselation_evaluation"];
		}
		else if (Engine::GetRenderer()->GetRenderApi() == RenderApiType::DX12) {
			vertexPath = shaderInfo["hlsl_shaders"]["vertex"];
			fragmentPath = shaderInfo["hlsl_shaders"]["fragment"];

			if (shaderInfo["hlsl_shaders"].contains("tesselation_control"))
				tesselationControlPath = shaderInfo["hlsl_shaders"]["tesselation_control"];

			if (shaderInfo["hlsl_shaders"].contains("tesselation_evaluation"))
				tesselationEvaluationPath = shaderInfo["hlsl_shaders"]["tesselation_evaluation"];
		}

		for (auto& slotInfo : shaderInfo["slots"]) {
			std::string slotName = slotInfo["name"];
			auto& slot = layout->GetSlot(slotName);
			slot.glslSetIndex = slotInfo["glsl_set"];

			for (auto& stage : slotInfo["shader_stages"])
				EFTraits::AddFlag(&slot.stage, GetShaderStage(stage));

			for (auto& bindingInfo : slotInfo["bindings"]) {
				std::string bindingName = bindingInfo["name"];

				slot.bindings.Get(bindingName).glslIndex = bindingInfo["glsl_index"];
				slot.bindings.Get(bindingName).hlslIndex = bindingInfo["hlsl_index"];
			}
		}

		for (auto& pushConstantInfo : shaderInfo["push_constants"]) {
			auto& pushConst = layout->GetPushConstant(pushConstantInfo["name"]);

			for (auto& stage : pushConstantInfo["shader_stages"])
				EFTraits::AddFlag(&pushConst.stage, GetShaderStage(stage));

			pushConst.hlslIndex = pushConstantInfo["hlsl_index"];
		}
	}

	TSize hlslDescIndex = 0;
	for (auto& setName : layout->GetAllSlotNames()) {
		auto& set = layout->GetSlot(setName);

		for (auto& binding : set.bindings) {
			binding.second.hlslDescriptorIndex = hlslDescIndex;

			hlslDescIndex++;
		}
	}

	for (auto& pushConstInfo : layout->GetAllPushConstants()) {
		pushConstInfo.second.hlslBindingIndex = hlslDescIndex;
		hlslDescIndex++;
	}

	PipelineCreateInfo info{};
	info.vertexPath = vertexPath;
	info.fragmentPath = fragmentPath;
	info.tesselationControlPath = tesselationControlPath;
	info.tesselationEvaluationPath = tesselationEvaluationPath;
	info.polygonMode = polygonMode;
	info.cullMode = PolygonCullMode::BACK;
	info.frontFaceType = PolygonFrontFaceType::COUNTERCLOCKWISE;

	if (materialInfo.find("config") != materialInfo.end()) {
		if (materialInfo["config"].contains("depth_testing")) {
			if (materialInfo["config"]["depth_testing"] == "none")
				info.depthTestingType = DepthTestingType::NONE;
			else if (materialInfo["config"]["depth_testing"] == "read")
				info.depthTestingType = DepthTestingType::READ;
			else if (materialInfo["config"]["depth_testing"] == "read/write")
				info.depthTestingType = DepthTestingType::READ_WRITE;
			else
				OSK_ASSERT(false, "Error en el archivo de material" + shaderFilePath + ": config depth_testing inválido.");
		}

		if (materialInfo["config"].contains("cull_mode")) {
			if (materialInfo["config"]["cull_mode"] == "none")
				info.cullMode = PolygonCullMode::NONE;
			else if (materialInfo["config"]["cull_mode"] == "front")
				info.cullMode = PolygonCullMode::FRONT;
			else if (materialInfo["config"]["cull_mode"] == "back")
				info.cullMode = PolygonCullMode::BACK;
			else
				OSK_ASSERT(false, "Error en el archivo de material" + shaderFilePath + ": config cull_mode inválido.");
		}
	}

	auto output = new Material(info, layout, vertexType);

	materials.Insert(output);
	for (auto& i : registeredRenderpasses)
		output->RegisterRenderpass(i);

	return output;
}

void MaterialSystem::RegisterRenderpass(const IRenderpass* renderpass) {
	registeredRenderpasses.Insert(renderpass);

	for (auto& i : materials)
		i->RegisterRenderpass(renderpass);
}

void MaterialSystem::UnregisterRenderpass(const IRenderpass* renderpass) {
	registeredRenderpasses.Remove(renderpass);

	for (auto& i : materials)
		i->UnregisterRenderpass(renderpass);
}
