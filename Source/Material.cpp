#include "Material.h"

#include "IGraphicsPipeline.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialInstance.h"
#include "MaterialLayout.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

Material::Material(const PipelineCreateInfo& pipelineInfo, OwnedPtr<MaterialLayout> layout, const VertexInfo& vertexInfo, MaterialType materialType)
	: pipelineInfo(pipelineInfo), vertexInfo(vertexInfo), materialType(materialType) {

	this->layout = layout.GetPointer();

	InitializePipelines();
}

void Material::SetName(const std::string& name) {
	this->name = name;
}

OwnedPtr<MaterialInstance> Material::CreateInstance() {
	MaterialInstance* output = new MaterialInstance(this);

	for (const auto& [name, slot] : layout->GetAllSlots())
		output->RegisterSlot(name);

	return output;
}

MaterialType Material::GetMaterialType() const {
	return materialType;
}

const PipelineCreateInfo& Material::GetInfo() const {
	return pipelineInfo;
}

std::string_view Material::GetName() const {
	return name;
}

const MaterialLayout* Material::GetLayout() const {
	return layout.GetPointer();
}

const IGraphicsPipeline* Material::GetGraphicsPipeline(const PipelineKey& key) const {
	for (UIndex64 i = 0; i < graphicsPipelines.GetSize(); i++) {
		const PipelineKey& iKey = graphicsPipelinesKeys[i];

		if (key == iKey)
			return graphicsPipelines[i].GetPointer();
	}

	// Crear nuevo pipeline compatible.
	PipelineCreateInfo newInfo = pipelineInfo; 
	newInfo.formats = key.colorFormats;
	newInfo.depthFormat = key.depthFormat;

	OwnedPtr<IGraphicsPipeline> output = Engine::GetRenderer()->_CreateGraphicsPipeline(newInfo, *GetLayout(), vertexInfo);
	output->SetDebugName(name);

	graphicsPipelines.Insert(output.GetPointer());
	graphicsPipelinesKeys.Insert(key);

	return output.GetPointer();
}

const IMeshPipeline* Material::GetMeshPipeline(const PipelineKey& key) const {
	for (UIndex64 i = 0; i < m_meshPipelines.GetSize(); i++) {
		const PipelineKey& iKey = m_meshPipelinesKeys[i];

		if (key == iKey)
			return m_meshPipelines[i].GetPointer();
	}

	// Crear nuevo pipeline compatible.
	PipelineCreateInfo newInfo = pipelineInfo;
	newInfo.formats = key.colorFormats;
	newInfo.depthFormat = key.depthFormat;

	OwnedPtr<IMeshPipeline> output = Engine::GetRenderer()->_CreateMeshPipeline(newInfo, *GetLayout());
	output->SetDebugName(name);

	m_meshPipelines.Insert(output.GetPointer());
	m_meshPipelinesKeys.Insert(key);

	return output.GetPointer();
}

const IRaytracingPipeline* Material::GetRaytracingPipeline() const {
	return rtPipeline.GetPointer();
}

const IComputePipeline* Material::GetComputePipeline() const {
	return computePipeline.GetPointer();
}

void Material::InitializePipelines() {
	switch (materialType) {
	case MaterialType::RAYTRACING:
		if (Engine::GetRenderer()->IsRtActive())
			rtPipeline = Engine::GetRenderer()->_CreateRaytracingPipeline(pipelineInfo, layout.GetValue(), vertexInfo).GetPointer();
		break;

	case MaterialType::COMPUTE:
		computePipeline = Engine::GetRenderer()->_CreateComputePipeline(pipelineInfo, layout.GetValue()).GetPointer();
	}
}

void Material::_Reload() {
	switch (materialType) {

	case MaterialType::GRAPHICS:
		// Basta con limpiar el cach� para que se
		// generen nuevos pipelines.
		graphicsPipelines.Empty();
		graphicsPipelinesKeys.Empty();
		break;

	case MaterialType::MESH:
		// Basta con limpiar el cach� para que se
		// generen nuevos pipelines.
		m_meshPipelines.Empty();
		m_meshPipelinesKeys.Empty();
		break;

	case MaterialType::RAYTRACING:
	case MaterialType::COMPUTE:
		rtPipeline.Delete();
		computePipeline.Delete();

		InitializePipelines();

		break;
	}
}
