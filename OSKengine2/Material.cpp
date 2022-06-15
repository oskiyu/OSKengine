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
}

Material::~Material() {
	for (auto& i : graphicsPipelines)
		delete i.second.GetPointer();
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

const IGraphicsPipeline* Material::GetGraphicsPipeline(const IRenderpass* renderpass) const {
	return graphicsPipelines.Get((IRenderpass*)renderpass).GetPointer();
}

const IRaytracingPipeline* Material::GetRaytracingPipeline() const {
	return rtPipeline.GetPointer();
}

void Material::RegisterRenderpass(const IRenderpass* renderpass) {
	if (!pipelineInfo.isRaytracing)
		graphicsPipelines.Insert(renderpass,
			Engine::GetRenderer()->_CreateGraphicsPipeline(pipelineInfo, layout.GetPointer(), renderpass, vertexInfo));
	else if (!rtPipeline.HasValue())
			rtPipeline = Engine::GetRenderer()->_CreateRaytracingPipeline(pipelineInfo, layout.GetPointer(), nullptr, vertexInfo).GetPointer();
}

void Material::UnregisterRenderpass(const IRenderpass* renderpass) {
	if (graphicsPipelines.ContainsKey(renderpass))
		delete graphicsPipelines.Get(renderpass).GetPointer();

	graphicsPipelines.Remove(renderpass);
}
