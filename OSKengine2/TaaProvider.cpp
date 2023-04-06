#include "TaaProvider.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialSystem.h"
#include "Material.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void TaaProvider::InitializeTaa(const Vector2ui& resolution, const GpuImage* sceneImages[NUM_RESOURCES_IN_FLIGHT], const GpuImage* motionImages[NUM_RESOURCES_IN_FLIGHT]) {
	RenderTargetAttachmentInfo taaAttachment{};
	taaAttachment.format = Format::RGBA16_SFLOAT;
	taaAttachment.name = "TAA Image";
	taaAttachment.sampler = GpuImageSamplerDesc::CreateDefault();
	taaAttachment.usage = GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION;

	taaRenderTarget.Create(resolution, taaAttachment);

	taaAttachment.name = "TAA Sharpened";

	taaSharpenedRenderTarget.Create(resolution, taaAttachment);

	LoadTaaMaterials();
	SetupTaaMaterials(sceneImages, motionImages);
}

void TaaProvider::ResizeTaa(const Vector2ui& resolution, const GpuImage* sceneImages[NUM_RESOURCES_IN_FLIGHT], const GpuImage* motionImages[NUM_RESOURCES_IN_FLIGHT]) {
	taaRenderTarget.Resize(resolution);
	taaSharpenedRenderTarget.Resize(resolution);

	SetupTaaMaterials(sceneImages, motionImages);
}

void TaaProvider::LoadTaaMaterials() {
	Material* taaMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/TAA/taa.json");
	Material* taaSharpenMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/TAA/sharpen.json");

	taaMaterialInstance = taaMaterial->CreateInstance().GetPointer();
	taaSharpenMaterialInstance = taaSharpenMaterial->CreateInstance().GetPointer();
}

void TaaProvider::SetupTaaMaterials(const GpuImage* sceneImages[NUM_RESOURCES_IN_FLIGHT],	const GpuImage* motionImages[NUM_RESOURCES_IN_FLIGHT]) {
	GpuImageViewConfig sampledViewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	GpuImageViewConfig storageViewConfig = GpuImageViewConfig::CreateStorage_Default();

	const IGpuImageView* _taaHistoricalImages[NUM_RESOURCES_IN_FLIGHT]{};
	const IGpuImageView* _taaTargetImages[NUM_RESOURCES_IN_FLIGHT]{};

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		const TIndex previousIndex = (i + NUM_RESOURCES_IN_FLIGHT - 1) % NUM_RESOURCES_IN_FLIGHT;

		_taaHistoricalImages[i] = taaRenderTarget.GetTargetImage(previousIndex)->GetView(sampledViewConfig);
		_taaTargetImages[i] = taaRenderTarget.GetTargetImage(i)->GetView(storageViewConfig);
	}

	auto sceneViews = GetViews(sceneImages, sampledViewConfig);
	auto motionViews = GetViews(motionImages, sampledViewConfig);

	taaMaterialInstance->GetSlot("global")->SetGpuImages("sceneImage", sceneViews.data());
	taaMaterialInstance->GetSlot("global")->SetGpuImages("historicalImage", _taaHistoricalImages);
	taaMaterialInstance->GetSlot("global")->SetGpuImages("velocityImage", motionViews.data());
	taaMaterialInstance->GetSlot("global")->SetStorageImages("finalImg", _taaTargetImages);
	taaMaterialInstance->GetSlot("global")->FlushUpdate();

	const IGpuImageView* _taaSharpenInput[NUM_RESOURCES_IN_FLIGHT]{};
	const IGpuImageView* _taaSharpenOutput[NUM_RESOURCES_IN_FLIGHT]{};

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		_taaSharpenInput[i] = taaRenderTarget.GetTargetImage(i)->GetView(sampledViewConfig);
		_taaSharpenOutput[i] = taaSharpenedRenderTarget.GetTargetImage(i)->GetView(storageViewConfig);
	}

	taaSharpenMaterialInstance->GetSlot("global")->SetGpuImages("taaImage", _taaSharpenInput);
	taaSharpenMaterialInstance->GetSlot("global")->SetStorageImages("finalImg", _taaSharpenOutput);
	taaSharpenMaterialInstance->GetSlot("global")->FlushUpdate();
}

void TaaProvider::ExecuteTaa(ICommandList* commandList) {
	if (!isActive) {
		currentFrameJitterIndex = maxJitterIndex;
		return;
	}

	currentFrameJitterIndex = currentFrameJitterIndex % maxJitterIndex + 1;

	ExecuteTaaFirstPass(commandList);
	ExecuteTaaSharpening(commandList);
}

void TaaProvider::ExecuteTaaFirstPass(ICommandList* commandList) {
	commandList->StartDebugSection("Temporal Accumulation", Color::PURPLE());

	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	const TIndex previousIndex = (resourceIndex + NUM_RESOURCES_IN_FLIGHT - 1) % NUM_RESOURCES_IN_FLIGHT;

	GpuImage* historicalImage = taaRenderTarget.GetTargetImage(previousIndex);
	GpuImage* finalImage = taaRenderTarget.GetTargetImage(resourceIndex);

	commandList->SetGpuImageBarrier(
		finalImage,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE));

	const Vector2ui resoulution = taaRenderTarget.GetSize();
	const Vector2ui threadGroupSize = { 8u, 8u };
	const Vector2ui dispatchRes = resoulution / threadGroupSize + Vector2ui(1u, 1u);

	commandList->BindMaterial(taaMaterialInstance->GetMaterial());
	commandList->BindMaterialSlot(taaMaterialInstance->GetSlot("global"));
	commandList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });

	commandList->SetGpuImageBarrier(
		finalImage,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE),
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));

	commandList->EndDebugSection();
}

void TaaProvider::ExecuteTaaSharpening(ICommandList* commandList) {
	commandList->StartDebugSection("Sharpening", Color::PURPLE());

	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	GpuImage* sharpenedImage = taaSharpenedRenderTarget.GetTargetImage(resourceIndex);

	commandList->SetGpuImageBarrier(
		sharpenedImage,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::DEFAULT, GpuBarrierAccessStage::DEFAULT),
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE));

	const Vector2ui resoulution = taaRenderTarget.GetSize();
	const Vector2ui threadGroupSize = { 8u, 8u };
	const Vector2ui dispatchRes = resoulution / threadGroupSize + Vector2ui(1u, 1u);

	commandList->BindMaterial(taaSharpenMaterialInstance->GetMaterial());
	commandList->BindMaterialSlot(taaSharpenMaterialInstance->GetSlot("global"));
	commandList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });

	commandList->EndDebugSection();
}

std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> TaaProvider::GetViews(const GpuImage* image[NUM_RESOURCES_IN_FLIGHT], GpuImageViewConfig config) {
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> output{};

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		output[i] = image[i]->GetView(config);

	return output;
}
