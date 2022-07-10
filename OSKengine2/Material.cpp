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

	if (!pipelineInfo.isRaytracing) {
		const Format pipelineFormat = pipelineInfo.isFinal
			? Engine::GetRenderer()->_GetSwapchain()->GetColorFormat()
			: Format::RGBA8_UNORM;

		graphicsPipeline = Engine::GetRenderer()->_CreateGraphicsPipeline(pipelineInfo, layout.GetPointer(), pipelineFormat, vertexInfo).GetPointer();
	}
	else {
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

const IGraphicsPipeline* Material::GetGraphicsPipeline() const {
	return graphicsPipeline.GetPointer();
}

const IRaytracingPipeline* Material::GetRaytracingPipeline() const {
	return rtPipeline.GetPointer();
}
