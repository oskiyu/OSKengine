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

	this->colorFormat = colorFormat;
	this->depthFormat = depthFormat;

	CreateTargetImages();
	CreateDepthImages();

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

	CreateTargetImages();
	CreateDepthImages();

	if (targetType == RenderpassType::INTERMEDIATE)
		UpdateSpriteImages();

	spriteTransform.SetScale(GetOriginalSize().ToVector2f());
}

void RenderTarget::CreateTargetImages() {
	for (TSize i = 0; i < NUM_RENDER_TARGET_IMAGES; i++) {
		GpuImage* image = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
			{ GetSize().X, GetSize().Y, 1 }, GpuImageDimension::d2D, 1, colorFormat,
			targetUsage, GpuSharedMemoryType::GPU_ONLY, 1
		).GetPointer();

		if (targetImages[i].IsEmpty())
			targetImages[i].Insert(image);
		else
			targetImages[i].At(0) = image;
	}
}

void RenderTarget::CreateDepthImages() {
	for (TSize i = 0; i < NUM_RENDER_TARGET_IMAGES; i++) {
		depthImages[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
			{ GetSize().X, GetSize().Y, 1 }, GpuImageDimension::d2D, 1, depthFormat,
			GpuImageUsage::DEPTH_STENCIL, GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();
	}
}

void RenderTarget::UpdateSpriteImages() {
	targetSprite.GetMaterialInstance()->GetSlot("texture")->SetGpuImage("stexture0", targetImages[0].At(0).GetPointer());
	targetSprite.GetMaterialInstance()->GetSlot("texture")->SetGpuImage("stexture1", targetImages[1].At(0).GetPointer());
	targetSprite.GetMaterialInstance()->GetSlot("texture")->SetGpuImage("stexture2", targetImages[2].At(0).GetPointer());
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
	return colorFormat;
}

Format RenderTarget::GetDepthFormat() const {
	return depthFormat;
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
