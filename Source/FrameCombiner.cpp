#include "FrameCombiner.h"

#include "NumericTypes.h"
#include "Vector2.hpp"
#include "Vector3.hpp"

#include "OwnedPtr.h"

#include "OSKengine.h"
#include "IRenderer.h"

#include "ICommandList.h"

#include "MaterialSystem.h"
#include "Material.h"
#include "MaterialInstance.h"

#include "IGpuImage.h"
#include "IGpuImageView.h"
#include "GpuImageViewConfig.h"
#include "GpuImageLayout.h"
#include "GpuImageUsage.h"

#include "RenderTargetAttachmentInfo.h"
#include "RtRenderTarget.h"


using namespace OSK;
using namespace OSK::GRAPHICS;

void FrameCombiner::Create(const Vector2ui& resolution, RenderTargetAttachmentInfo info) {
	info.usage |= GpuImageUsage::SAMPLED | GpuImageUsage::COMPUTE | GpuImageUsage::TRANSFER_DESTINATION;

	m_renderTarget.Create(resolution, info);

	LoadMaterial();
	SetupTargetMaterial();
}

void FrameCombiner::Resize(const Vector2ui& resolution) {
	m_renderTarget.Resize(resolution);
	SetupTargetMaterial();

	// Borramos la caché de instancias,
	// para no referenciar imágenes eliminadas.
	m_textureMaterials.clear();
}


void FrameCombiner::LoadMaterial() {
	MaterialSystem* materialSystem = Engine::GetRenderer()->GetMaterialSystem();

	m_combinerMaterials[ImageFormat::RGBA8]  = materialSystem->LoadMaterial("Resources/Materials/2D/FrameCombiner/fcombiner8.json");
	m_combinerMaterials[ImageFormat::RGBA16] = materialSystem->LoadMaterial("Resources/Materials/2D/FrameCombiner/fcombiner16.json");

	m_outputMaterialInstance = m_combinerMaterials.at(ImageFormat::RGBA8)->CreateInstance().GetPointer();
}

void FrameCombiner::SetupTargetMaterial() {
	const GpuImageViewConfig targetViewConfig = GpuImageViewConfig::CreateStorage_Default();
	const IGpuImageView& view = *m_renderTarget.GetTargetImage()->GetView(targetViewConfig);

	m_outputMaterialInstance->GetSlot("output")->SetStorageImage("outputImage", view);
	m_outputMaterialInstance->GetSlot("output")->FlushUpdate();
}

void FrameCombiner::SetupTextureMaterialInstance(const IGpuImageView& image) {
	OwnedPtr<MaterialInstance> materialInstance = m_combinerMaterials.at(ImageFormat::RGBA8)->CreateInstance();
	materialInstance->GetSlot("input")->SetGpuImage("inputImage", image);
	materialInstance->GetSlot("input")->FlushUpdate();

	m_textureMaterials[&image] = materialInstance.GetPointer();
}


void FrameCombiner::Begin(ICommandList* commandList, ImageFormat format) {
	auto* targetImage = m_renderTarget.GetTargetImage();

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

	commandList->BindMaterial(*m_combinerMaterials.at(format));
	commandList->BindMaterialInstance(m_outputMaterialInstance.GetValue());
}

void FrameCombiner::Draw(ICommandList* commandList, const IGpuImageView& image) {
	if (!m_textureMaterials.contains(&image))
		SetupTextureMaterialInstance(image);

	commandList->BindMaterialSlot(*m_textureMaterials.at(&image)->GetSlot("input"));
	commandList->DispatchCompute(GetDispatchResolution());
}

void FrameCombiner::End(ICommandList*) {

}

GpuBarrierInfo FrameCombiner::GetImageBarrierInfo() {
	return GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ);
}

GpuImage* FrameCombiner::GetTargetImage() {
	return m_renderTarget.GetTargetImage();
}

const GpuImage* FrameCombiner::GetTargetImage() const {
	return m_renderTarget.GetTargetImage();
}

ComputeRenderTarget& FrameCombiner::GetRenderTarget() {
	return m_renderTarget;
}

const ComputeRenderTarget& FrameCombiner::GetRenderTarget() const {
	return m_renderTarget;
}

Vector3ui FrameCombiner::GetDispatchResolution() const {
	const static Vector2ui groupSize = { 8u, 8u };
	return Vector3ui(
		static_cast<USize32>(glm::ceil(m_renderTarget.GetSize().x / groupSize.x)),
		static_cast<USize32>(glm::ceil(m_renderTarget.GetSize().y / groupSize.y)),
		1u
	);
}
