#include "Material.h"

#include "IGraphicsPipeline.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialInstance.h"
#include "MaterialLayout.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

Material::Material(const PipelineCreateInfo& pipelineInfo, OwnedPtr<MaterialLayout> layout) 
	: pipelineInfo(pipelineInfo) {

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

const MaterialLayout* Material::GetLayout() const {
	return layout.GetPointer();
}

const IGraphicsPipeline* Material::GetGraphicsPipeline(const IRenderpass* renderpass) const {
	return graphicsPipelines.Get((IRenderpass*)renderpass).GetPointer();
}

void Material::RegisterRenderpass(const IRenderpass* renderpass) {
	graphicsPipelines.Insert(renderpass,
		Engine::GetRenderer()->_CreateGraphicsPipeline(pipelineInfo, layout.GetPointer(), renderpass));
}

void Material::UnregisterRenderpass(const IRenderpass* renderpass) {
	delete graphicsPipelines.Get(renderpass).GetPointer();

	graphicsPipelines.Remove(renderpass);
}
