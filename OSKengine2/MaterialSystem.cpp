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
		nlohmann::json materialInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(path));

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

		for (auto& pushConstantInfo : materialInfo["layout"]["push_constants"]) {
			MaterialLayoutPushConstant pushConst{};

			pushConst.name = pushConstantInfo["name"];
			pushConst.size = pushConstantInfo["size"];

			layout->AddPushConstant(pushConst);
		}
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
	info.polygonMode = PolygonMode::FILL;
	info.cullMode = PolygonCullMode::FRONT;
	info.frontFaceType = PolygonFrontFaceType::CLOCKWISE;

	return new Material(info, layout);
}
