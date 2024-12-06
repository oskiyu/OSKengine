#include "ToneMapping.h"

#include "OSKengine.h"
#include "NumericTypes.h"
#include "IRenderer.h"
#include "ResourcesInFlight.h"

#include "ICommandList.h"
#include "IPostProcessPass.h"

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Color.hpp"

#include "MaterialSystem.h"
#include "Material.h"

#include "GpuImageLayout.h"
#include "GpuBuffer.h"

#include <glm/glm.hpp>
#include <span>
#include "GpuBarrierInfo.h"

#include "PostProcessExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void ToneMappingPass::Create(const Vector2ui& size) {
	IPostProcessPass::Create(size);

	m_material = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(MaterialPath);

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateTarget_Color();

	for (auto& mInstance : m_materialInstances) {
		mInstance = m_material->CreateInstance().GetPointer();

		mInstance->GetSlot(TextureSlotName)->SetStorageImage("finalImage", *GetOutput().GetTargetImage()->GetView(viewConfig));
		mInstance->GetSlot(TextureSlotName)->FlushUpdate();
	}
}

void ToneMappingPass::Resize(const Vector2ui& size) {
	IPostProcessPass::Resize(size);

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateTarget_Color();
	for (auto& mInstance : m_materialInstances) {
		mInstance = m_material->CreateInstance().GetPointer();

		mInstance->GetSlot(TextureSlotName)->SetStorageImage("finalImage", *GetOutput().GetTargetImage()->GetView(viewConfig));
		mInstance->GetSlot(TextureSlotName)->FlushUpdate();
	}
}

void ToneMappingPass::Execute(ICommandList* computeCmdList) {
	OSK_ASSERT(
		m_inputImage != nullptr,
		PostProcessInputNotSetException("No se ha establecido la entrada de color."));

	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	computeCmdList->StartDebugSection("Tone Mapping", Color::Purple);

	computeCmdList->SetGpuImageBarrier(
		m_inputImage,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ),
		GpuImageRange{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = 1 });

	computeCmdList->SetGpuImageBarrier(
		GetOutput().GetTargetImage(),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*m_material);
	computeCmdList->BindMaterialSlot(*m_materialInstances[resourceIndex]->GetSlot(TextureSlotName));
	computeCmdList->BindMaterialSlot(*m_materialInstances[resourceIndex]->GetSlot(ExposureSlotName));

	const Vector3ui dispatchRes = {
		static_cast<USize32>(glm::ceil(static_cast<float>(GetOutput().GetSize().x) / static_cast<float>(DispatchResolution.x))),
		static_cast<USize32>(glm::ceil(static_cast<float>(GetOutput().GetSize().y) / static_cast<float>(DispatchResolution.y))),
		1
	};

	struct { float exposure; float gamma; } const pushConstants{ m_exposure, m_gamma - 1.2f };
	computeCmdList->PushMaterialConstants(PushConstantsName, pushConstants);
	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->EndDebugSection();
}

void ToneMappingPass::SetExposure(float exposure) {
	m_exposure = exposure;
}

float ToneMappingPass::GetExposure() const {
	return m_exposure;
}

void ToneMappingPass::SetGamma(float gamma) {
	m_gamma = gamma;
}

float ToneMappingPass::GetGamma() const {
	return m_gamma;
}

void ToneMappingPass::SetExposureBuffer(const GpuBuffer& buffer) {
	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		m_materialInstances[i]->GetSlot(ExposureSlotName)->SetStorageBuffer(ExposureBindingName, buffer);
		m_materialInstances[i]->GetSlot(ExposureSlotName)->FlushUpdate();
	}
}

void ToneMappingPass::SetInput(GpuImage* input) {
	m_inputImage = input;

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);

	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		m_materialInstances[i]->GetSlot(TextureSlotName)->SetGpuImage(
			"sceneImage", 
			*input->GetView(viewConfig), 
			GpuImageSamplerDesc::CreateDefault_NoMipMap());
		m_materialInstances[i]->GetSlot(TextureSlotName)->FlushUpdate();
	}
}
