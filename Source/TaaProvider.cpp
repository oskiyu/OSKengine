#include "TaaProvider.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "CopyImageInfo.h"
#include "Format.h"
#include "GpuImageUsage.h"
#include "GpuImageSamplerDesc.h"
#include "IGpuImage.h"
#include "GpuBarrierInfo.h"
#include "GpuImageViewConfig.h"
#include "Color.hpp"
#include "IGpuImageView.h"
#include "RenderTargetAttachmentInfo.h"
#include "IMaterialSlot.h"
#include "ICommandList.h"
#include "GpuImageLayout.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void TaaProvider::InitializeTaa(const Vector2ui& resolution, const GpuImage* sceneImage, const GpuImage* motionImage) {
	RenderTargetAttachmentInfo taaAttachment{};
	taaAttachment.format = Format::RGBA16_SFLOAT;
	taaAttachment.name = "TAA Image";
	taaAttachment.sampler = GpuImageSamplerDesc::CreateDefault();
	taaAttachment.usage = GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION;

	m_taaHistoricalImage.Create(resolution, taaAttachment);
	m_taaRenderTarget.Create(resolution, taaAttachment);

	taaAttachment.name = "TAA Sharpened";

	m_taaSharpenedRenderTarget.Create(resolution, taaAttachment);

	LoadTaaMaterials();
	SetupTaaMaterials(sceneImage, motionImage);
}

void TaaProvider::ResizeTaa(
	const Vector2ui& resolution, 
	const GpuImage* sceneImage, 
	const GpuImage* motionImage) 
{
	m_taaHistoricalImage.Resize(resolution);
	m_taaRenderTarget.Resize(resolution);
	m_taaSharpenedRenderTarget.Resize(resolution);

	SetupTaaMaterials(sceneImage, motionImage);
}

void TaaProvider::LoadTaaMaterials() {
	Material* taaMaterial		 = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(MaterialPath);
	Material* taaSharpenMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(SharpeningMaterialPath);

	m_taaMaterialInstance = taaMaterial->CreateInstance().GetPointer();
	m_taaSharpenMaterialInstance = taaSharpenMaterial->CreateInstance().GetPointer();
}

void TaaProvider::SetupTaaMaterials(const GpuImage* sceneImage, const GpuImage* motionImage) {
	const auto sampledViewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	const auto storageViewConfig = GpuImageViewConfig::CreateStorage_Default();

	const auto& sceneView		= *sceneImage->GetView(sampledViewConfig);
	const auto& historicalView	= *m_taaHistoricalImage.GetTargetImage()->GetView(sampledViewConfig);
	const auto& motionView		= *motionImage->GetView(sampledViewConfig);
	const auto& targetView		= *m_taaRenderTarget.GetTargetImage()->GetView(storageViewConfig);

	IMaterialSlot* const taaGlobalSlot = m_taaMaterialInstance->GetSlot(GlobalSlotName);
	taaGlobalSlot->SetGpuImage(TaaInputImageBindingName, sceneView);
	taaGlobalSlot->SetGpuImage(TaaHistoricalImageBindingName, historicalView);
	taaGlobalSlot->SetGpuImage(TaaMotionImageBindingName, motionView);
	taaGlobalSlot->SetStorageImage(TaaOutputImageBindingName, targetView);
	taaGlobalSlot->FlushUpdate();


	const IGpuImageView& sharpenInputview  = *m_taaRenderTarget.GetTargetImage()->GetView(sampledViewConfig);
	const IGpuImageView& sharpenOutputview = *m_taaSharpenedRenderTarget.GetTargetImage()->GetView(storageViewConfig);

	IMaterialSlot* const sharpeningGlobalSlot = m_taaSharpenMaterialInstance->GetSlot(GlobalSlotName);
	sharpeningGlobalSlot->SetGpuImage(SharpeningInputImageBindingName, sharpenInputview);
	sharpeningGlobalSlot->SetStorageImage(SharpeningOutputImageBindingName, sharpenOutputview);
	sharpeningGlobalSlot->FlushUpdate();
}

void TaaProvider::ExecuteTaa(ICommandList* commandList) {
	if (!m_isActive) {
		m_currentFrameJitterIndex = m_maxJitterIndex;
		return;
	}

	m_currentFrameJitterIndex = (m_currentFrameJitterIndex + 1) % m_maxJitterIndex;

	commandList->StartDebugSection(TaaDebugName, Color::Purple);
	
	ExecuteTaaFirstPass(commandList);
	CopyToHistoricalImage(commandList);
	ExecuteTaaSharpening(commandList);

	commandList->EndDebugSection();
}

void TaaProvider::ExecuteTaaFirstPass(ICommandList* commandList) {
	commandList->StartDebugSection(TaaAccumulationDebugName, Color::Purple);

	GpuImage* historicalImage = m_taaHistoricalImage.GetTargetImage();
	GpuImage* finalImage = m_taaRenderTarget.GetTargetImage();

	const GpuImageLayout previousLayout = historicalImage->_GetLayout(0, 0);
	commandList->SetGpuImageBarrier(
		historicalImage,
		previousLayout,
		GpuImageLayout::SAMPLED, 
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	commandList->SetGpuImageBarrier(
		finalImage,
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	const Vector2ui resoulution = m_taaRenderTarget.GetSize();
	const Vector2ui threadGroupSize = { 8u, 8u };
	const Vector2ui dispatchRes = resoulution / threadGroupSize + Vector2ui(1u, 1u);

	commandList->BindMaterial(*m_taaMaterialInstance->GetMaterial());
	commandList->BindMaterialSlot(*m_taaMaterialInstance->GetSlot(GlobalSlotName));
	commandList->DispatchCompute({ dispatchRes.x, dispatchRes.y, 1 });

	commandList->EndDebugSection();
}

void TaaProvider::CopyToHistoricalImage(ICommandList* commandList) {
	commandList->StartDebugSection(TaaHistoryCopyDebugName, Color::Yellow);

	GpuImage* sourceImage = m_taaRenderTarget.GetTargetImage();
	GpuImage* destinationImage = m_taaHistoricalImage.GetTargetImage();

	commandList->SetGpuImageBarrier(
		sourceImage,
		GpuImageLayout::TRANSFER_SOURCE,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ));

	commandList->SetGpuImageBarrier(
		destinationImage,
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(m_taaRenderTarget.GetSize());

	commandList->RawCopyImageToImage(
		*m_taaRenderTarget.GetTargetImage(),
		m_taaHistoricalImage.GetTargetImage(),
		copyInfo);

	commandList->EndDebugSection();
}

void TaaProvider::ExecuteTaaSharpening(ICommandList* commandList) {
	commandList->StartDebugSection(TaaSharpeningDebugName, Color::Purple);

	GpuImage* sourceImage = m_taaRenderTarget.GetTargetImage();
	GpuImage* sharpenedImage = m_taaSharpenedRenderTarget.GetTargetImage();

	commandList->SetGpuImageBarrier(
		sourceImage,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	commandList->SetGpuImageBarrier(
		sharpenedImage,
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	const Vector2ui resoulution = m_taaRenderTarget.GetSize();
	const Vector2ui threadGroupSize = ThreadGroupSize;
	const Vector2ui dispatchRes = resoulution / threadGroupSize + Vector2ui(1u, 1u);

	commandList->BindMaterial(*m_taaSharpenMaterialInstance->GetMaterial());
	commandList->BindMaterialSlot(*m_taaSharpenMaterialInstance->GetSlot(GlobalSlotName));
	commandList->DispatchCompute({ dispatchRes.x, dispatchRes.y, 1 });

	commandList->EndDebugSection();
}
