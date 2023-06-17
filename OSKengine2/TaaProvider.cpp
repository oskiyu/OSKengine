#include "TaaProvider.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialSystem.h"
#include "Material.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void TaaProvider::InitializeTaa(const Vector2ui& resolution, std::span<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> sceneImages, std::span<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> motionImages) {
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

void TaaProvider::ResizeTaa(
	const Vector2ui& resolution, 
	std::span<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> sceneImages, 
	std::span<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> motionImages) 
{
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

void TaaProvider::SetupTaaMaterials(std::span<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> sceneImages, std::span<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> motionImages) {
	const auto sampledViewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	const auto storageViewConfig = GpuImageViewConfig::CreateStorage_Default();

	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> _taaHistoricalImages{};
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> _taaTargetImages{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		const UIndex32 previousIndex = (i + NUM_RESOURCES_IN_FLIGHT - 1) % NUM_RESOURCES_IN_FLIGHT;

		_taaHistoricalImages[i] = taaRenderTarget.GetTargetImage(previousIndex)->GetView(sampledViewConfig);
		_taaTargetImages[i] = taaRenderTarget.GetTargetImage(i)->GetView(storageViewConfig);
	}

	auto sceneViews = GetViews(sceneImages, sampledViewConfig);
	auto motionViews = GetViews(motionImages, sampledViewConfig);

	taaMaterialInstance->GetSlot("global")->SetGpuImages("sceneImage", sceneViews);
	taaMaterialInstance->GetSlot("global")->SetGpuImages("historicalImage", _taaHistoricalImages);
	taaMaterialInstance->GetSlot("global")->SetGpuImages("velocityImage", motionViews);
	taaMaterialInstance->GetSlot("global")->SetStorageImages("finalImg", _taaTargetImages);
	taaMaterialInstance->GetSlot("global")->FlushUpdate();

	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> _taaSharpenInput{};
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> _taaSharpenOutput{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
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
	commandList->StartDebugSection("Temporal Accumulation", Color::Purple);

	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	const UIndex32 previousIndex = (resourceIndex + NUM_RESOURCES_IN_FLIGHT - 1) % NUM_RESOURCES_IN_FLIGHT;

	GpuImage* historicalImage = taaRenderTarget.GetTargetImage(previousIndex);
	GpuImage* finalImage = taaRenderTarget.GetTargetImage(resourceIndex);

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

	const Vector2ui resoulution = taaRenderTarget.GetSize();
	const Vector2ui threadGroupSize = { 8u, 8u };
	const Vector2ui dispatchRes = resoulution / threadGroupSize + Vector2ui(1u, 1u);

	commandList->BindMaterial(*taaMaterialInstance->GetMaterial());
	commandList->BindMaterialSlot(*taaMaterialInstance->GetSlot("global"));
	commandList->DispatchCompute({ dispatchRes.x, dispatchRes.y, 1 });

	commandList->EndDebugSection();
}

void TaaProvider::ExecuteTaaSharpening(ICommandList* commandList) {
	commandList->StartDebugSection("Sharpening", Color::Purple);

	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	GpuImage* sourceImage = taaRenderTarget.GetTargetImage(resourceIndex);
	GpuImage* sharpenedImage = taaSharpenedRenderTarget.GetTargetImage(resourceIndex);

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

	const Vector2ui resoulution = taaRenderTarget.GetSize();
	const Vector2ui threadGroupSize = { 8u, 8u };
	const Vector2ui dispatchRes = resoulution / threadGroupSize + Vector2ui(1u, 1u);

	commandList->BindMaterial(*taaSharpenMaterialInstance->GetMaterial());
	commandList->BindMaterialSlot(*taaSharpenMaterialInstance->GetSlot("global"));
	commandList->DispatchCompute({ dispatchRes.x, dispatchRes.y, 1 });

	commandList->EndDebugSection();
}

std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> TaaProvider::GetViews(
	std::span<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> image, 
	GpuImageViewConfig config) 
{
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> output{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		output[i] = image[i]->GetView(config);

	return output;
}
