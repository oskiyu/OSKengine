#include "RenderTarget.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "Format.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypes.h"
#include "Material.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void RenderTarget::Create(const Vector2ui& targetSize, Format colorFormat, Format depthFormat) {
	this->size = targetSize;

	AddColorTarget(colorFormat, targetUsage, colorSampler);
	CreateDepthImages(depthFormat);

	// Sprite
	targetSprite.SetMaterialInstance(Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_rendertarget.json")->CreateInstance());
	targetSprite.SetCamera(Engine::GetRenderer()->GetRenderTargetsCamera());
	targetSprite.SetTexCoords({ { 0.0f, 0.0f, 1.0f, 1.0f }, TextureCoordsType::NORMALIZED });

	if (targetType == RenderpassType::INTERMEDIATE)
		UpdateSpriteImages();

	spriteTransform.SetScale(GetOriginalSize().ToVector2f());
}

void RenderTarget::Resize(const Vector2ui& targetSize) {
	this->size = targetSize;

	RecreateTargetImages();
	RecreateDepthImages();

	if (targetType == RenderpassType::INTERMEDIATE)
		UpdateSpriteImages();

	spriteTransform.SetScale(GetOriginalSize().ToVector2f());
}

void RenderTarget::AddColorTarget(Format colorFormat, GpuImageUsage usage, GpuImageSamplerDesc sampler) {
	for (TSize i = 0; i < NUM_RENDER_TARGET_IMAGES; i++) {
		GpuImage* image = Engine::GetRenderer()->GetAllocator()->CreateImage(
			{ GetSize().X, GetSize().Y, 1 }, GpuImageDimension::d2D, 1, colorFormat,
			usage, GpuSharedMemoryType::GPU_ONLY, 1, sampler).GetPointer();

		targetImages[i].Insert(image);
	}
}

void RenderTarget::RecreateTargetImages() {
	for (TSize i = 0; i < NUM_RENDER_TARGET_IMAGES; i++) {

		for (TSize img = 0; img < targetImages[i].GetSize(); img++) {
			const Format imgFormat = targetImages[i][img]->GetFormat();
			const GpuImageUsage imgUsage = targetImages[i][img]->GetUsage();
			const GpuImageSamplerDesc sampler = targetImages[i][img]->GetImageSampler();

			targetImages[i][img] = Engine::GetRenderer()->GetAllocator()->CreateImage(
				{ GetSize().X, GetSize().Y, 1 }, GpuImageDimension::d2D, 1, imgFormat,
				imgUsage, GpuSharedMemoryType::GPU_ONLY, 1, sampler).GetPointer();
		}

	}
}

void RenderTarget::CreateDepthImages(Format format) {
	for (TSize i = 0; i < NUM_RENDER_TARGET_IMAGES; i++) {
		depthImages[i] = Engine::GetRenderer()->GetAllocator()->CreateImage(
			{ GetSize().X, GetSize().Y, 1 }, GpuImageDimension::d2D, 1, format,
			depthUsage, GpuSharedMemoryType::GPU_ONLY, 1, depthSampler).GetPointer();
	}
}

void RenderTarget::RecreateDepthImages() {
	CreateDepthImages(depthImages[0]->GetFormat());
}

void RenderTarget::UpdateSpriteImages() {
	const GpuImage* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = targetImages[i].At(0).GetPointer();

	targetSprite.GetMaterialInstance()->GetSlot("texture")->SetGpuImages("stexture", images);
	targetSprite.GetMaterialInstance()->GetSlot("texture")->FlushUpdate();
}

GpuImage* RenderTarget::GetMainTargetImage(TSize index) const {
	OSK_ASSERT(index >= 0, "El índice de la imagen debe estar entre 0 y " + std::to_string(NUM_RENDER_TARGET_IMAGES - 1));
	OSK_ASSERT(index < NUM_RENDER_TARGET_IMAGES, "El índice de la imagen debe estar entre 0 y " + std::to_string(NUM_RENDER_TARGET_IMAGES - 1));

	return targetImages[index].At(0).GetPointer();
}

DynamicArray<GpuImage*> RenderTarget::GetTargetImages(TSize index) const {
	OSK_ASSERT(index >= 0, "El índice de la imagen debe estar entre 0 y " + std::to_string(NUM_RENDER_TARGET_IMAGES - 1));
	OSK_ASSERT(index < NUM_RENDER_TARGET_IMAGES, "El índice de la imagen debe estar entre 0 y " + std::to_string(NUM_RENDER_TARGET_IMAGES - 1));

	DynamicArray<GpuImage*> output = DynamicArray<GpuImage*>::CreateReservedArray(targetImages[index].GetSize());
	for (auto& img : targetImages[index])
		output.Insert(img.GetPointer());

	return output;
}

GpuImage* RenderTarget::GetDepthImage(TSize index) const {
	OSK_ASSERT(index >= 0, "El índice de la imagen debe estar entre 0 y " + std::to_string(NUM_RENDER_TARGET_IMAGES - 1));
	OSK_ASSERT(index < NUM_RENDER_TARGET_IMAGES, "El índice de la imagen debe estar entre 0 y " + std::to_string(NUM_RENDER_TARGET_IMAGES - 1));

	return depthImages[index].GetPointer();
}

const Sprite& RenderTarget::GetSprite() const {
	return targetSprite;
}

const ECS::Transform2D& RenderTarget::GetSpriteTransform() const {
	return spriteTransform;
}

Vector2ui RenderTarget::GetSize() const {
	return (size.ToVector2f() * resolutionScale).ToVector2ui();
}

Vector2ui RenderTarget::GetOriginalSize() const {
	return size;
}

Format RenderTarget::GetColorFormat() const {
	return targetImages[0][0]->GetFormat();
}

Format RenderTarget::GetDepthFormat() const {
	return depthImages[0]->GetFormat();
}

RenderpassType RenderTarget::GetRenderTargetType() const {
	return targetType;
}

void RenderTarget::SetRenderTargetType(RenderpassType type) {
	targetType = type;
}

void RenderTarget::SetResolutionScale(float scale) {
	OSK_ASSERT(scale > 0.0f, "La escala del render target debe ser > 0.");

	resolutionScale = scale;
	Resize(GetOriginalSize());
}

void RenderTarget::SetTargetImageUsage(GpuImageUsage usage) {
	targetUsage = GpuImageUsage::COLOR | GpuImageUsage::SAMPLED | usage;
}

void RenderTarget::SetDepthImageUsage(GpuImageUsage usage) {
	depthUsage = GpuImageUsage::DEPTH_STENCIL | usage;
}

void RenderTarget::SetColorImageSampler(const GpuImageSamplerDesc& sampler) {
	colorSampler = sampler;
}

void RenderTarget::SetDepthImageSampler(const GpuImageSamplerDesc& sampler) {
	depthSampler = sampler;
}

TSize RenderTarget::GetNumColorTargets() const {
	return targetImages[0].GetSize();
}

void RenderTarget::SetName(const std::string& name) {
	for (TSize i = 0; i < 3; i++)
		for (TSize img = 0; img < targetImages[i].GetSize(); img++)
			targetImages[i][img]->SetDebugName(name + " attachment " + std::to_string(img) + "[" + std::to_string(i) + "]");

	for (TSize i = 0; i < 3; i++)
			depthImages[i]->SetDebugName(name + " depth[" + std::to_string(i) + "]");
}
