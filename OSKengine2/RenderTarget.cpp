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

void RenderTarget::Create(const Vector2ui& targetSize) {
	targetImage = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
		{ targetSize.X, targetSize.Y, 1 }, GpuImageDimension::d2D, 1, Format::RGBA8_UNORM,
		GpuImageUsage::COLOR | GpuImageUsage::SAMPLED, GpuSharedMemoryType::GPU_ONLY, 1
	).GetPointer();

	targetRenderpass = Engine::GetRenderer()->CreateSecondaryRenderpass(targetImage.GetPointer()).GetPointer();

	// Sprite
	targetSprite.SetMaterialInstance(Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_2d.json")->CreateInstance());
	targetSprite.SetCamera(Engine::GetRenderer()->GetRenderTargetsCamera());
	targetSprite.SetGpuImage(targetImage.GetPointer());
	targetSprite.SetTexCoords({ { 0.0f, 0.0f, 1.0f, 1.0f }, TextureCoordsType::NORMALIZED });

	spriteTransform.SetScale(targetSize.ToVector2f());
}

void RenderTarget::Resize(const Vector2ui& targetSize) {
	targetImage = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
		{ targetSize.X, targetSize.Y, 1 }, GpuImageDimension::d2D, 1, Format::RGBA8_UNORM,
		GpuImageUsage::COLOR | GpuImageUsage::SAMPLED, GpuSharedMemoryType::GPU_ONLY, 1
	).GetPointer();

	targetRenderpass->SetImages(
		targetImage.GetPointer(),
		targetImage.GetPointer(),
		targetImage.GetPointer()
	);

	spriteTransform.SetScale(targetSize.ToVector2f());
}

GpuImage* RenderTarget::GetTargetImage() const {
	return targetImage.GetPointer();
}

IRenderpass* RenderTarget::GetTargetRenderpass() const {
	return targetRenderpass.GetPointer();
}

const Sprite& RenderTarget::GetSprite() const {
	return targetSprite;
}

const ECS::Transform2D& RenderTarget::GetSpriteTransform() const {
	return spriteTransform;
}
