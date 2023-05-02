#include "FxaaPass.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "GpuImageLayout.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void FxaaPass::Create(const Vector2ui& size) {
	postProcessingMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/fxaa.json");
	postProcessingMaterialInstance = postProcessingMaterial->CreateInstance().GetPointer();

	IPostProcessPass::Create(size);

	// resolveRenderTarget.SetName("FXAA Output");
}

void FxaaPass::Execute(ICommandList* computeCmdList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	computeCmdList->StartDebugSection("FXAA", Color::PURPLE());

	computeCmdList->SetGpuImageBarrier(
		inputImages[resourceIndex], 
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(resourceIndex), 
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*postProcessingMaterial);
	computeCmdList->BindMaterialSlot(*postProcessingMaterialInstance->GetSlot("texture"));

	const Vector3ui dispatchRes = {
		static_cast<TSize>(glm::ceil(resolveRenderTarget.GetSize().X / 8.0f)),
		static_cast<TSize>(glm::ceil(resolveRenderTarget.GetSize().Y / 8.0f)),
		1
	};

	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->EndDebugSection();
}
