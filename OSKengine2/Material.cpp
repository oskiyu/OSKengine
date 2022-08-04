#include "Material.h"

#include "IGraphicsPipeline.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialInstance.h"
#include "MaterialLayout.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

Material::Material(const PipelineCreateInfo& pipelineInfo, OwnedPtr<MaterialLayout> layout, const VertexInfo& vertexInfo)
	: pipelineInfo(pipelineInfo), vertexInfo(vertexInfo) {

	this->layout = layout.GetPointer();

	if (pipelineInfo.isRaytracing && Engine::GetRenderer()->IsRtActive()) {
		rtPipeline = Engine::GetRenderer()->_CreateRaytracingPipeline(pipelineInfo, layout.GetPointer(), vertexInfo).GetPointer();
	}
}

Material::~Material() {

}

OwnedPtr<MaterialInstance> Material::CreateInstance() {
	MaterialInstance* output = new MaterialInstance(this);

	for (const auto& i : layout->GetAllSlotNames())
		output->RegisterSlot(i);

	return output;
}

bool Material::IsRaytracing() const {
	return pipelineInfo.isRaytracing;
}

const MaterialLayout* Material::GetLayout() const {
	return layout.GetPointer();
}

const IGraphicsPipeline* Material::GetGraphicsPipeline(const PipelineKey& key) {
	for (TSize i = 0; i < graphicsPipelines.GetSize(); i++) {
		const PipelineKey& iKey = graphicsPipelines[i].first;

		if (key.GetSize() != iKey.GetSize())
			continue;

		bool compatible = true;
		for (TSize j = 0; j < key.GetSize(); j++){
			if (key[j] != iKey[j]) {
				compatible = false;
				continue;
			}
		}
		
		if (!compatible)
			continue;
		
		// Compatible
		return graphicsPipelines[i].second.GetPointer();
	}

	// Crear nuevo pipeline compatible.
	pipelineInfo.formats = key;
	OwnedPtr<IGraphicsPipeline> output = Engine::GetRenderer()->_CreateGraphicsPipeline(pipelineInfo, GetLayout(), vertexInfo);

	graphicsPipelines.Insert({ key, output.GetPointer() });

	return output.GetPointer();
}

const IRaytracingPipeline* Material::GetRaytracingPipeline() const {
	return rtPipeline.GetPointer();
}
