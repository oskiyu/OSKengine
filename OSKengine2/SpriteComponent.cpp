#include "SpriteComponent.h"

#include "Texture.h"
#include "CameraComponent2D.h"
#include "IMaterialSlot.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

Sprite& SpriteComponent::GetSprite() {
	return sprite;
}

const Sprite& SpriteComponent::GetSprite() const {
	return sprite;
}

void SpriteComponent::SetMaterialInstance(MaterialInstance* mInstance) {
	materialInstance = mInstance;
}

void SpriteComponent::SetCamera(const CameraComponent2D& camera) {
	if (materialInstance.HasValue()) {
		materialInstance->GetSlot("global")->SetUniformBuffer("camera", camera.GetUniformBuffer());
		materialInstance->GetSlot("global")->FlushUpdate();
	}
	else {
		OSK_CHECK(false, "No se ha establecido la instanca del material del sprite.");
	}
}

MaterialInstance* SpriteComponent::GetMaterialInstance() const {
	return materialInstance.GetPointer();
}

void SpriteComponent::SetTexture(const Texture* texture) {
	SetGpuImage(texture->GetGpuImage());
}

void SpriteComponent::SetGpuImage(const GpuImage* image) {
	sprite.SetGpuImage(image);
	if (materialInstance.HasValue()) {
		materialInstance->GetSlot("texture")->SetGpuImage("texture", image);
		materialInstance->GetSlot("texture")->FlushUpdate();
	}
	else {
		OSK_CHECK(false, "No se ha establecido la instanca del material del sprite.");
	}
}
