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
	computeCmdList->StartDebugSection("FXAA", Color::Purple);

	computeCmdList->SetGpuImageBarrier(
		inputImage, 
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(), 
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*postProcessingMaterial);
	computeCmdList->BindMaterialSlot(*postProcessingMaterialInstance->GetSlot("texture"));

	const Vector3ui dispatchRes = {
		static_cast<UIndex32>(glm::ceil(static_cast<float>(resolveRenderTarget.GetSize().x) / 8.0f)),
		static_cast<UIndex32>(glm::ceil(static_cast<float>(resolveRenderTarget.GetSize().y) / 8.0f)),
		1
	};

	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->EndDebugSection();
}
