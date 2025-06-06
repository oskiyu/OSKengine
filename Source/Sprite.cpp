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
#include "Math.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;
using namespace OSK::PERSISTENCE;


UniquePtr<GpuBuffer> Sprite::globalVertexBuffer;
UniquePtr<GpuBuffer> Sprite::globalIndexBuffer;


Sprite::Sprite() {
	SetupMaterial(Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/2D/material_2d.json"));
}

void Sprite::SetupMaterial(Material* material) {
	OSK_ASSERT(material->GetLayout()->GetAllSlots().contains("texture"), InvalidMaterialException());
	textureMaterialInstance = material->CreateInstance();
}

void Sprite::SetTexCoords(const TextureCoordinates2D& texCoords) {
	this->texCoords = texCoords;
}

TextureCoordinates2D Sprite::GetTexCoords() const {
	return texCoords;
}

void Sprite::LinkAsset(ASSETS::AssetRef<Texture> texture) {
	m_texture = texture;
}

void Sprite::SetImageView(const IGpuImageView* view) {
	this->view = view;

	if (!view)
		return;

	textureMaterialInstance->GetSlot("texture")->SetGpuImage("stexture", *view, GpuImageSamplerDesc::CreateDefault_NoMipMap());
	textureMaterialInstance->GetSlot("texture")->FlushUpdate();
}

const IMaterialSlot* Sprite::GetTextureSlot() const {
	return textureMaterialInstance->GetSlot("texture");
}

const IGpuImageView* Sprite::GetView() const {
	return view;
}

template <>
nlohmann::json PERSISTENCE::SerializeComponent<OSK::GRAPHICS::Sprite>(const OSK::GRAPHICS::Sprite& data) {
	nlohmann::json output{};

	return output;
}

template <>
OSK::GRAPHICS::Sprite PERSISTENCE::DeserializeComponent<OSK::GRAPHICS::Sprite>(const nlohmann::json& json, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	return {};
}


template <>
BinaryBlock PERSISTENCE::BinarySerializeComponent<OSK::GRAPHICS::Sprite>(const OSK::GRAPHICS::Sprite& data) {
	return BinaryBlock::Empty();
}

template <>
OSK::GRAPHICS::Sprite PERSISTENCE::BinaryDeserializeComponent<OSK::GRAPHICS::Sprite>(BinaryBlockReader* reader, const OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator) {
	return {};
}
