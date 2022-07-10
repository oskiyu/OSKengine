#include "MaterialSystem.h"

#include "FileIO.h"
#include "MaterialLayout.h"

#include "Material.h"
#include "PipelineCreateInfo.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "RenderApiType.h"

#include <spirv_cross/spirv_glsl.hpp>

using namespace OSK;
using namespace OSK::IO;
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
		return ShaderStage::TESSELATION_ALL;

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

void MaterialSystem::LoadMaterialV0(MaterialLayout* layout, const nlohmann::json& materialInfo, PipelineCreateInfo* info) {
	OSK_ASSERT(materialInfo.contains("shader_file"), "Archivo de material incorrecto: no se encuentra 'shader_file'.");
	OSK_ASSERT(materialInfo.contains("layout"), "Archivo de material incorrecto: no se encuentra 'layout'.");

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

	if (materialInfo.contains("polygon_mode"))
		info->polygonMode = GetPolygonMode(materialInfo["polygon_mode"]);

	// Shader file.
	{
		nlohmann::json shaderInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(shaderFilePath));

		int fileVersion = shaderInfo["spec_ver"];

		if (Engine::GetRenderer()->GetRenderApi() == RenderApiType::OPENGL
			|| Engine::GetRenderer()->GetRenderApi() == RenderApiType::VULKAN) {
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

}

void LoadSpirvCrossShader(MaterialLayout* layout, const nlohmann::json& materialInfo, const DynamicArray<char>& bytecode, ShaderStage stage, TSize* pushConstantsOffset, TSize* numBuffers, TSize* numImages, TSize* numBindings) {
	spirv_cross::Compiler compiler((const uint32_t*)bytecode.GetData(), bytecode.GetSize() / 4);
	spirv_cross::ShaderResources resources = compiler.get_shader_resources();

	// Imágenes
	for (auto& i : resources.sampled_images) {
		// Obtenemos el nombre del set (slot).
		// Si no tiene nombre registrado en el archivo, su nombre será su número.
		std::string setName = std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet));
		if (materialInfo.contains("slots") && materialInfo["slots"].contains(std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet))))
			setName = materialInfo["slots"][std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet))];

		// Si el slot no está registrado, registrarlo
		if (!layout->GetAllSlotNames().ContainsElement(setName)) {
			MaterialLayoutSlot slot{};
			slot.name = setName;
			slot.glslSetIndex = compiler.get_decoration(i.id, spv::DecorationDescriptorSet);
			slot.stage = stage;

			layout->AddSlot(slot);
		}
		else {
			EFTraits::AddFlag(&layout->GetSlot(setName).stage, stage);
		}

		MaterialLayoutBinding binding{};
		binding.name = compiler.get_name(i.id);
		binding.glslIndex = compiler.get_decoration(i.id, spv::DecorationBinding);
		binding.hlslIndex = *numImages;
		binding.hlslDescriptorIndex = *numBindings;

		(*numImages)++;
		(*numBindings)++;

		// Tipo de imagen
		const auto& imageInfo = compiler.get_type(i.type_id).image;
		if (imageInfo.dim == spv::Dim::DimCube)
			binding.type = ShaderBindingType::CUBEMAP;
		else
			binding.type = ShaderBindingType::TEXTURE;

		layout->GetSlot(setName).bindings.Insert(binding.name, binding);
	}

	// Buffers
	for (auto& i : resources.uniform_buffers) {
		// Obtenemos el nombre del set (slot).
		// Si no tiene nombre registrado en el archivo, su nombre será su número.
		std::string setName = std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet));
		if (materialInfo.contains("slots") && materialInfo["slots"].contains(std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet))))
			setName = materialInfo["slots"][std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet))];

		// Si el slot no está registrado, registrarlo
		if (!layout->GetAllSlotNames().ContainsElement(setName)) {
			MaterialLayoutSlot slot{};
			slot.name = setName;
			slot.glslSetIndex = compiler.get_decoration(i.id, spv::DecorationDescriptorSet);
			slot.stage = stage;

			layout->AddSlot(slot);
		}
		else {
			EFTraits::AddFlag(&layout->GetSlot(setName).stage, stage);
		}

		MaterialLayoutBinding binding{};
		binding.name = compiler.get_name(i.id);
		binding.glslIndex = compiler.get_decoration(i.id, spv::DecorationBinding);
		binding.type = ShaderBindingType::UNIFORM_BUFFER;
		binding.hlslIndex = *numBuffers;
		binding.hlslDescriptorIndex = *numBindings;

		(*numBuffers)++;
		(*numBindings)++;

		// Tamaño del buffer
		const auto& bufferSize = compiler.get_declared_struct_size(compiler.get_type(i.type_id));

		layout->GetSlot(setName).bindings.Insert(binding.name, binding);
	}

	for (auto& i : resources.acceleration_structures) {
		// Obtenemos el nombre del set (slot).
		// Si no tiene nombre registrado en el archivo, su nombre será su número.
		std::string setName = std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet));
		if (materialInfo.contains("slots") && materialInfo["slots"].contains(std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet))))
			setName = materialInfo["slots"][std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet))];

		// Si el slot no está registrado, registrarlo
		if (!layout->GetAllSlotNames().ContainsElement(setName)) {
			MaterialLayoutSlot slot{};
			slot.name = setName;
			slot.glslSetIndex = compiler.get_decoration(i.id, spv::DecorationDescriptorSet);
			slot.stage = stage;

			layout->AddSlot(slot);
		}
		else {
			EFTraits::AddFlag(&layout->GetSlot(setName).stage, stage);
		}

		MaterialLayoutBinding binding{};
		binding.name = compiler.get_name(i.id);
		binding.glslIndex = compiler.get_decoration(i.id, spv::DecorationBinding);
		binding.type = ShaderBindingType::RT_ACCELERATION_STRUCTURE;
		//binding.hlslIndex = *numBuffers;
		//binding.hlslDescriptorIndex = *numBindings; TODO

		//(*numBuffers)++;
		//(*numBindings)++;

		layout->GetSlot(setName).bindings.Insert(binding.name, binding);
	}

	for (auto& i : resources.storage_images) {
		// Obtenemos el nombre del set (slot).
		// Si no tiene nombre registrado en el archivo, su nombre será su número.
		std::string setName = std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet));
		if (materialInfo.contains("slots") && materialInfo["slots"].contains(std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet))))
			setName = materialInfo["slots"][std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet))];

		// Si el slot no está registrado, registrarlo
		if (!layout->GetAllSlotNames().ContainsElement(setName)) {
			MaterialLayoutSlot slot{};
			slot.name = setName;
			slot.glslSetIndex = compiler.get_decoration(i.id, spv::DecorationDescriptorSet);
			slot.stage = stage;

			layout->AddSlot(slot);
		}
		else {
			EFTraits::AddFlag(&layout->GetSlot(setName).stage, stage);
		}

		MaterialLayoutBinding binding{};
		binding.name = compiler.get_name(i.id);
		binding.glslIndex = compiler.get_decoration(i.id, spv::DecorationBinding);
		binding.type = ShaderBindingType::RT_TARGET_IMAGE;
		//binding.hlslIndex = *numBuffers;
		//binding.hlslDescriptorIndex = *numBindings; TODO

		//(*numBuffers)++;
		//(*numBindings)++;

		layout->GetSlot(setName).bindings.Insert(binding.name, binding);
	}
	for (auto& i : resources.storage_buffers) {
		// Obtenemos el nombre del set (slot).
		// Si no tiene nombre registrado en el archivo, su nombre será su número.
		std::string setName = std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet));
		if (materialInfo.contains("slots") && materialInfo["slots"].contains(std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet))))
			setName = materialInfo["slots"][std::to_string(compiler.get_decoration(i.id, spv::DecorationDescriptorSet))];

		// Si el slot no está registrado, registrarlo
		if (!layout->GetAllSlotNames().ContainsElement(setName)) {
			MaterialLayoutSlot slot{};
			slot.name = setName;
			slot.glslSetIndex = compiler.get_decoration(i.id, spv::DecorationDescriptorSet);
			slot.stage = stage;

			layout->AddSlot(slot);
		}
		else {
			EFTraits::AddFlag(&layout->GetSlot(setName).stage, stage);
		}

		MaterialLayoutBinding binding{};
		binding.name = compiler.get_name(i.id);
		binding.glslIndex = compiler.get_decoration(i.id, spv::DecorationBinding);
		binding.type = ShaderBindingType::STORAGE_BUFFER;
		//binding.hlslIndex = *numBuffers;
		//binding.hlslDescriptorIndex = *numBindings; TODO

		//(*numBuffers)++;
		//(*numBindings)++;

		layout->GetSlot(setName).bindings.Insert(binding.name, binding);
	}

	// PushConstants
	for (auto& i : resources.push_constant_buffers) {

		// Si el push constant ya fue definido, actualizar su stage.
		bool found = false;
		for (auto& pConst : layout->GetAllPushConstants()) {
			if (pConst.second.name == compiler.get_name(i.id)) {
				EFTraits::AddFlag(&pConst.second.stage, stage);

				found = true;
			}
		}

		if (found)
			continue;

		MaterialLayoutPushConstant pushConstantInfo{};
		pushConstantInfo.name = compiler.get_name(i.id);
		pushConstantInfo.stage = stage;
		pushConstantInfo.size = static_cast<TSize>(compiler.get_declared_struct_size(compiler.get_type(i.type_id)));
		pushConstantInfo.offset = *pushConstantsOffset;
		pushConstantInfo.hlslIndex = *numBuffers;
		pushConstantInfo.hlslBindingIndex = *numBindings;

		(*numBuffers)++;
		(*numBindings)++;

		*pushConstantsOffset += pushConstantInfo.offset;

		layout->AddPushConstant(pushConstantInfo);
	}
}

void MaterialSystem::LoadMaterialV1(MaterialLayout* layout, const nlohmann::json& materialInfo, PipelineCreateInfo* info) {
	TSize pushConstantsOffset = 0;

	TSize numHlslBuffers = 0;
	TSize numHlslImages = 0;
	TSize numHlslBindings = 0;

	info->precompiledHlslShaders = false;

	if (info->isRaytracing) {
		info->rtRaygenShaderPath = materialInfo["rt_raygen_shader"];
		info->rtClosestHitShaderPath = materialInfo["rt_closesthit_shader"];
		info->rtMissShaderPath = materialInfo["rt_miss_shader"];

		LoadSpirvCrossShader(layout, materialInfo, FileIO::ReadBinaryFromFile(materialInfo["rt_raygen_shader"]), ShaderStage::RT_RAYGEN, &pushConstantsOffset, &numHlslBuffers, &numHlslImages, &numHlslBindings);
		LoadSpirvCrossShader(layout, materialInfo, FileIO::ReadBinaryFromFile(materialInfo["rt_closesthit_shader"]), ShaderStage::RT_CLOSEST_HIT, &pushConstantsOffset, &numHlslBuffers, &numHlslImages, &numHlslBindings);
		LoadSpirvCrossShader(layout, materialInfo, FileIO::ReadBinaryFromFile(materialInfo["rt_miss_shader"]), ShaderStage::RT_MISS, &pushConstantsOffset, &numHlslBuffers, &numHlslImages, &numHlslBindings);
	}
	else {
		info->vertexPath = materialInfo["vertex_shader"];
		info->fragmentPath = materialInfo["fragment_shader"];

		LoadSpirvCrossShader(layout, materialInfo, FileIO::ReadBinaryFromFile(materialInfo["vertex_shader"]), ShaderStage::VERTEX, &pushConstantsOffset, &numHlslBuffers, &numHlslImages, &numHlslBindings);
		LoadSpirvCrossShader(layout, materialInfo, FileIO::ReadBinaryFromFile(materialInfo["fragment_shader"]), ShaderStage::FRAGMENT, &pushConstantsOffset, &numHlslBuffers, &numHlslImages, &numHlslBindings);
		if (materialInfo.contains("tesselation_control_shader")) {
			LoadSpirvCrossShader(layout, materialInfo, FileIO::ReadBinaryFromFile(materialInfo["tesselation_control_shader"]), ShaderStage::TESSELATION_CONTROL, &pushConstantsOffset, &numHlslBuffers, &numHlslImages, &numHlslBindings);
			info->tesselationControlPath = materialInfo["tesselation_control_shader"];
		}
		if (materialInfo.contains("tesselation_evaluation_shader")) {
			LoadSpirvCrossShader(layout, materialInfo, FileIO::ReadBinaryFromFile(materialInfo["tesselation_evaluation_shader"]), ShaderStage::TESSELATION_EVALUATION, &pushConstantsOffset, &numHlslBuffers, &numHlslImages, &numHlslBindings);
			info->tesselationEvaluationPath = materialInfo["tesselation_evaluation_shader"];
		}
	}

	TSize nextHlslBinding = 0;
	for (const auto& slotName : layout->GetAllSlotNames()) {
		for (auto& binding : layout->GetSlot(slotName).bindings) {
			binding.second.hlslDescriptorIndex = nextHlslBinding;
			nextHlslBinding++;
		}
	}

	for (auto& pushConstant : layout->GetAllPushConstants()) {
		pushConstant.second.hlslBindingIndex = nextHlslBinding;
		nextHlslBinding++;
	}
}

Material* MaterialSystem::LoadMaterial(const std::string& path) {
	if (materialsTable.ContainsKey(path))
		return materialsTable.Get(path);

	MaterialLayout* layout = new MaterialLayout;

	// Material file.
	nlohmann::json materialInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(path));

	OSK_ASSERT(materialInfo.contains("file_type"), "Archivo de material incorrecto: no se encuentra 'file_type'.");
	OSK_ASSERT(materialInfo.contains("name"), "Archivo de material incorrecto: no se encuentra 'name'.");
	OSK_ASSERT(materialInfo.contains("vertex_type"), "Archivo de material incorrecto: no se encuentra 'vertex_type'.");

	bool isRaytracing = false;
	if (materialInfo["file_type"] == "MATERIAL_RT") {
		OSK_ASSERT(Engine::GetRenderer()->SupportsRaytracing(), "No se puede cargar un material de raytracing.");
		isRaytracing = true;
	}
	else {
		OSK_ASSERT(materialInfo["file_type"] == "MATERIAL", std::string("Archivo ") + path + "no es un material.");
	}
	
	const std::string vertexName = materialInfo["vertex_type"];
	const VertexInfo& vertexType = vertexTypesTable.Get(vertexName);

	int fileVersion = materialInfo["spec_ver"];

	PipelineCreateInfo info{};
	info.cullMode = PolygonCullMode::BACK;
	info.frontFaceType = PolygonFrontFaceType::COUNTERCLOCKWISE;
	info.isRaytracing = isRaytracing;

	if (fileVersion == 0)
		LoadMaterialV0(layout, materialInfo, &info);
	else if (fileVersion == 1)
		LoadMaterialV1(layout, materialInfo, &info);
	else
		OSK_ASSERT(false, "La versión del archivo de material json no está soportada (" + std::to_string(fileVersion) + ").");

	if (materialInfo.find("config") != materialInfo.end()) {
		if (materialInfo["config"].contains("depth_testing")) {
			if (materialInfo["config"]["depth_testing"] == "none")
				info.depthTestingType = DepthTestingType::NONE;
			else if (materialInfo["config"]["depth_testing"] == "read")
				info.depthTestingType = DepthTestingType::READ;
			else if (materialInfo["config"]["depth_testing"] == "read/write")
				info.depthTestingType = DepthTestingType::READ_WRITE;
			else
				OSK_ASSERT(false, "Error en el archivo de material" + path + ": config depth_testing inválido.");
		}

		if (materialInfo["config"].contains("cull_mode")) {
			if (materialInfo["config"]["cull_mode"] == "none")
				info.cullMode = PolygonCullMode::NONE;
			else if (materialInfo["config"]["cull_mode"] == "front")
				info.cullMode = PolygonCullMode::FRONT;
			else if (materialInfo["config"]["cull_mode"] == "back")
				info.cullMode = PolygonCullMode::BACK;
			else
				OSK_ASSERT(false, "Error en el archivo de material" + path + ": config cull_mode inválido.");
		}

		if (materialInfo["config"].contains("is_final")) {
			if (materialInfo["config"]["is_final"] == "true")
				info.isFinal = true;
		}
	}

	auto output = new Material(info, layout, vertexType);

	materials.Insert(output);

	materialsTable.Insert(path, output);

	return output;
}
