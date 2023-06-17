#include "Sprite.h"

#include "IGpuImage.h"
#include "Assert.h"
#include "MaterialInstance.h"
#include "IMaterialSlot.h"
#include "Texture.h"
#include "CameraComponent2D.h"
#include "Material.h"

#include "OSKengine.h"
#include "MaterialSystem.h"

#include "MaterialExceptions.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

GpuBuffer* Sprite::globalVertexBuffer = nullptr;
GpuBuffer* Sprite::globalIndexBuffer = nullptr;


Sprite::Sprite() {
	SetupMaterial(Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/2D/material_2d.json"));
}

void Sprite::SetupMaterial(Material* material) {
	OSK_ASSERT(material->GetLayout()->GetAllSlots().ContainsKey("texture"), InvalidMaterialException());
	textureMaterialInstance = material->CreateInstance().GetPointer();
}

void Sprite::SetTexCoords(const TextureCoordinates2D& texCoords) {
	this->texCoords = texCoords;
}

TextureCoordinates2D Sprite::GetTexCoords() const {
	return texCoords;
}

void Sprite::SetImageView(const IGpuImageView* view) {
	this->view = view;

	textureMaterialInstance->GetSlot("texture")->SetGpuImage("stexture", view);
	textureMaterialInstance->GetSlot("texture")->FlushUpdate();
}

const IMaterialSlot* Sprite::GetTextureSlot() const {
	return textureMaterialInstance->GetSlot("texture");
}

const IGpuImageView* Sprite::GetView() const {
	return view;
}
