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

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		textureMaterials[i].Clear();
}


void FrameCombiner::LoadMaterial() {
	MaterialSystem* materialSystem = Engine::GetRenderer()->GetMaterialSystem();

	combinerMaterials.Insert(ImageFormat::RGBA8, materialSystem->LoadMaterial("Resources/Materials/2D/FrameCombiner/fcombiner8.json"));
	combinerMaterials.Insert(ImageFormat::RGBA16, materialSystem->LoadMaterial("Resources/Materials/2D/FrameCombiner/fcombiner16.json"));

	outputMaterialInstance = combinerMaterials.Get(ImageFormat::RGBA8)->CreateInstance().GetPointer();
}

void FrameCombiner::SetupTargetMaterial() {
	const IGpuImageView* images[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImageViewConfig targetViewConfig = GpuImageViewConfig::CreateStorage_Default();

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = renderTarget.GetTargetImage(i)->GetView(targetViewConfig);

	outputMaterialInstance->GetSlot("output")->SetStorageImages("outputImage", images);
	outputMaterialInstance->GetSlot("output")->FlushUpdate();
}

void FrameCombiner::SetupTextureMaterialInstance(const IGpuImageView& image) {
	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	OwnedPtr<MaterialInstance> materialInstance = combinerMaterials.Get(ImageFormat::RGBA8)->CreateInstance();
	materialInstance->GetSlot("input")->SetGpuImage("inputImage", &image);
	materialInstance->GetSlot("input")->FlushUpdate();

	textureMaterials[resourceIndex].Insert(&image, materialInstance.GetPointer());
}


void FrameCombiner::Begin(ICommandList* commandList, ImageFormat format) {
	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

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

	commandList->BindMaterial(*combinerMaterials.Get(format));
	commandList->BindMaterialInstance(outputMaterialInstance.GetValue());
}

void FrameCombiner::Draw(ICommandList* commandList, const IGpuImageView& image) {
	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	if (!textureMaterials[resourceIndex].ContainsKey(&image))
		SetupTextureMaterialInstance(image);

	commandList->BindMaterialSlot(*textureMaterials[resourceIndex].Get(&image)->GetSlot("input"));
	commandList->DispatchCompute(GetDispatchResolution());
}

void FrameCombiner::End(ICommandList* commandList) {

}

GpuBarrierInfo FrameCombiner::GetImageBarrierInfo() {
	return GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ);
}

GpuImage* FrameCombiner::GetCurrentImage() const {
	return GetTargetImage(Engine::GetRenderer()->GetCurrentResourceIndex());
}

GpuImage* FrameCombiner::GetTargetImage(TIndex imageIndex) const {
	return renderTarget.GetTargetImage(imageIndex);
}

const ComputeRenderTarget& FrameCombiner::GetRenderTarget() const {
	return renderTarget;
}

Vector3ui FrameCombiner::GetDispatchResolution() const {
	const static Vector2ui groupSize = { 8u, 8u };
	return Vector3ui(
		glm::ceil(renderTarget.GetSize().X / groupSize.X),
		glm::ceil(renderTarget.GetSize().Y / groupSize.Y),
		1
	);
}
