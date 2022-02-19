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


ShaderBindingType GetShaderBindingType(const std::string& type) {
	if (type == "UNIFORM")
		return ShaderBindingType::UNIFORM_BUFFER;

	return ShaderBindingType::TEXTURE;
}

ShaderStage GetShaderStage(const std::string& type) {
	if (type == "VERTEX")
		return ShaderStage::VERTEX;

	return ShaderStage::FRAGMENT;
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

	// Material file.
	{
		nlohmann::json materialInfo = nlohmann::json::parse(FileIO::ReadFromFile(path));

		OSK_ASSERT(materialInfo["file_type"] == "MATERIAL", path + "no es un material.");
		
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
	}

	// Shader file.
	{
		nlohmann::json shaderInfo = nlohmann::json::parse(FileIO::ReadFromFile(shaderFilePath));

		//OSK_ASSERT(shaderInfo["file_type"] == "SHADER", shaderFilePath + "no es un shader.");
		
		int fileVersion = shaderInfo["spec_ver"];

		if (Engine::GetRenderer()->GetRenderApi() == RenderApiType::OPENGL
			|| Engine::GetRenderer()->GetRenderApi() == RenderApiType::VULKAN) {
			vertexPath = shaderInfo["glsl_shaders"]["vertex"];
			fragmentPath = shaderInfo["glsl_shaders"]["fragment"];
		}
		else if (Engine::GetRenderer()->GetRenderApi() == RenderApiType::DX12) {
			vertexPath = shaderInfo["hlsl_shaders"]["vertex"];
			fragmentPath = shaderInfo["hlsl_shaders"]["fragment"];
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
	}

	PipelineCreateInfo info{};
	info.vertexPath = vertexPath;
	info.fragmentPath = fragmentPath;
	info.polygonMode = PolygonMode::FILL;
	info.cullMode = PolygonCullMode::NONE;
	info.frontFaceType = PolygonFrontFaceType::CLOCKWISE;

	return new Material(info, layout);
}
