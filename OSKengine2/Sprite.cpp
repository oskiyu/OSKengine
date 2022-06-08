#include "Sprite.h"

#include "IGpuImage.h"
#include "Assert.h"
#include "MaterialInstance.h"
#include "IMaterialSlot.h"
#include "Texture.h"
#include "CameraComponent2D.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

IGpuVertexBuffer* Sprite::globalVertexBuffer = nullptr;
IGpuIndexBuffer* Sprite::globalIndexBuffer = nullptr;

void Sprite::SetTexCoords(const TextureCoordinates2D& texCoords) {
	this->texCoords = texCoords;
}

TextureCoordinates2D Sprite::GetTexCoords() const {
	return texCoords;
}

void Sprite::SetTexture(const Texture* texture) {
	SetGpuImage(texture->GetGpuImage());
}

void Sprite::SetCamera(const CameraComponent2D& camera) {
	if (materialInstance.HasValue()) {
		materialInstance->GetSlot("global")->SetUniformBuffer("camera", camera.GetUniformBuffer());
		materialInstance->GetSlot("global")->FlushUpdate();
	}
	else {
		OSK_CHECK(false, "No se ha establecido la instanca del material del sprite.");
	}
}

void Sprite::SetGpuImage(const GpuImage* image) {
	this->image = image;

	if (materialInstance.HasValue()) {
		materialInstance->GetSlot("texture")->SetGpuImage("stexture", image);
		materialInstance->GetSlot("texture")->FlushUpdate();
	}
	else {
		OSK_CHECK(false, "No se ha establecido la instanca del material del sprite.");
	}
}

const GpuImage* Sprite::GetGpuImage() const {
	return image;
}

void Sprite::SetMaterialInstance(OwnedPtr<MaterialInstance> instance) {
	this->materialInstance = instance.GetPointer();
}

MaterialInstance* Sprite::GetMaterialInstance() const {
	return materialInstance.GetPointer();
}
