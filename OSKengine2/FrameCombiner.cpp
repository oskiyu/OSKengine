#include "FrameCombiner.h"

#include "ICommandList.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "Material.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void FrameCombiner::Create(const Vector2ui& resolution, RenderTargetAttachmentInfo info) {
	info.usage |= GpuImageUsage::SAMPLED | GpuImageUsage::COMPUTE | GpuImageUsage::TRANSFER_DESTINATION;

	renderTarget.Create(resolution, info);

	LoadMaterial();
	SetupTargetMaterial();
}

void FrameCombiner::Resize(const Vector2ui& resolution) {
	renderTarget.Resize(resolution);
	SetupTargetMaterial();

	for (auto& material : textureMaterials)
		material.clear();
}


void FrameCombiner::LoadMaterial() {
	MaterialSystem* materialSystem = Engine::GetRenderer()->GetMaterialSystem();

	combinerMaterials[ImageFormat::RGBA8] = materialSystem->LoadMaterial("Resources/Materials/2D/FrameCombiner/fcombiner8.json");
	combinerMaterials[ImageFormat::RGBA16] = materialSystem->LoadMaterial("Resources/Materials/2D/FrameCombiner/fcombiner16.json");

	outputMaterialInstance = combinerMaterials.at(ImageFormat::RGBA8)->CreateInstance().GetPointer();
}

void FrameCombiner::SetupTargetMaterial() {
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images{};
	const GpuImageViewConfig targetViewConfig = GpuImageViewConfig::CreateStorage_Default();

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = renderTarget.GetTargetImage(i)->GetView(targetViewConfig);

	outputMaterialInstance->GetSlot("output")->SetStorageImages("outputImage", images);
	outputMaterialInstance->GetSlot("output")->FlushUpdate();
}

void FrameCombiner::SetupTextureMaterialInstance(const IGpuImageView& image) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	OwnedPtr<MaterialInstance> materialInstance = combinerMaterials.at(ImageFormat::RGBA8)->CreateInstance();
	materialInstance->GetSlot("input")->SetGpuImage("inputImage", &image);
	materialInstance->GetSlot("input")->FlushUpdate();

	textureMaterials[resourceIndex][&image] = materialInstance.GetPointer();
}


void FrameCombiner::Begin(ICommandList* commandList, ImageFormat format) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	auto targetImage = renderTarget.GetTargetImage(resourceIndex);

	commandList->SetGpuImageBarrier(
		targetImage,
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	commandList->ClearImage(targetImage);
	
	commandList->SetGpuImageBarrier(
		targetImage,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	commandList->BindMaterial(*combinerMaterials.at(format));
	commandList->BindMaterialInstance(outputMaterialInstance.GetValue());
}

void FrameCombiner::Draw(ICommandList* commandList, const IGpuImageView& image) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	if (!textureMaterials[resourceIndex].contains(&image))
		SetupTextureMaterialInstance(image);

	commandList->BindMaterialSlot(*textureMaterials[resourceIndex].at(&image)->GetSlot("input"));
	commandList->DispatchCompute(GetDispatchResolution());
}

void FrameCombiner::End(ICommandList*) {

}

GpuBarrierInfo FrameCombiner::GetImageBarrierInfo() {
	return GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ);
}

GpuImage* FrameCombiner::GetCurrentImage() const {
	return GetTargetImage(Engine::GetRenderer()->GetCurrentResourceIndex());
}

GpuImage* FrameCombiner::GetTargetImage(UIndex32 imageIndex) const {
	return renderTarget.GetTargetImage(imageIndex);
}

const ComputeRenderTarget& FrameCombiner::GetRenderTarget() const {
	return renderTarget;
}

Vector3ui FrameCombiner::GetDispatchResolution() const {
	const static Vector2ui groupSize = { 8u, 8u };
	return Vector3ui(
		static_cast<USize32>(glm::ceil(renderTarget.GetSize().x / groupSize.x)),
		static_cast<USize32>(glm::ceil(renderTarget.GetSize().y / groupSize.y)),
		1u
	);
}
