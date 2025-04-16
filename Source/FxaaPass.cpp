#include "FxaaPass.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialSystem.h"
#include "GpuImageLayout.h"
#include "Vector2.hpp"
#include "IPostProcessPass.h"
#include "ICommandList.h"
#include "NumericTypes.h"
#include "Vector3.hpp"
#include "Color.hpp"
#include "GpuBarrierInfo.h"
#include "Assert.h"
#include "PostProcessExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void FxaaPass::Create(const Vector2ui& size) {
	m_material = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(MaterialName);
	m_materialInstance = m_material->CreateInstance();

	IPostProcessPass::Create(size);
}

void FxaaPass::Execute(ICommandList* computeCmdList) {
	OSK_ASSERT(
		m_inputImage != nullptr,
		PostProcessInputNotSetException("No se ha establecido la entrada de color."));

	computeCmdList->StartDebugSection("FXAA", Color::Purple);

	computeCmdList->SetGpuImageBarrier(
		m_inputImage,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		GetOutput().GetTargetImage(),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*m_material);
	computeCmdList->BindMaterialSlot(*m_materialInstance->GetSlot("texture"));

	const Vector3ui dispatchRes = {
		static_cast<UIndex32>(glm::ceil(static_cast<float>(GetOutput().GetSize().x) / 8.0f)),
		static_cast<UIndex32>(glm::ceil(static_cast<float>(GetOutput().GetSize().y) / 8.0f)),
		1
	};

	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->EndDebugSection();
}

void FxaaPass::SetInput(GpuImage* image) {
	m_inputImage = image;
}
