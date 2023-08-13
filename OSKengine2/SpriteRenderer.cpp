#include "SpriteRenderer.h"

#include "PushConst2D.h"
#include "Sprite.h"
#include "Transform2D.h"
#include "ICommandList.h"
#include "MaterialInstance.h"
#include "MaterialLayout.h"
#include "IMaterialSlot.h"
#include "CameraComponent2D.h"
#include "Texture.h"
#include "Material.h"
#include "Font.h"
#include "FontInstance.h"
#include "FontCharacter.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"

#include "InvalidObjectStateException.h"
#include "InvalidArgumentException.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void SpriteRenderer::SetCommandList(ICommandList* commandList) noexcept {
	targetCommandList = commandList;
}

void SpriteRenderer::SetMaterial(const Material& material) {
	targetCommandList->BindMaterial(material);
}

void SpriteRenderer::SetMaterialSlot(const IMaterialSlot& materialSlot) {
	targetCommandList->BindMaterialSlot(materialSlot);
}

void SpriteRenderer::SetCamera(const CameraComponent2D& camera, const Transform2D& cameraTransform) {
	currentCameraMatrix = camera.GetProjection(cameraTransform);
}

void SpriteRenderer::SetCamera(GameObjectIndex gameObject) {
	SetCamera(
		Engine::GetEcs()->GetComponent<CameraComponent2D>(gameObject),
		Engine::GetEcs()->GetComponent<Transform2D>(gameObject));
}

void SpriteRenderer::Begin() {
	OSK_ASSERT(targetCommandList, InvalidObjectStateException("No se ha establecido la lista de comandos. Véase: SpriteRenderer::SetCommandList()."));

	targetCommandList->BindVertexBuffer(*Sprite::globalVertexBuffer);
	targetCommandList->BindIndexBuffer(*Sprite::globalIndexBuffer);

	isStarted = true;
}

void SpriteRenderer::Draw(const GRAPHICS::Sprite& sprite, const ECS::Transform2D& transform, const Color& color) {
	Draw(sprite, sprite.GetTexCoords(), transform, color);
}

void SpriteRenderer::Draw(const Sprite& sprite, const TextureCoordinates2D& texCoords, const ECS::Transform2D& transform, const Color& color) {
	OSK_ASSERT(targetCommandList, InvalidObjectStateException("No se ha establecido la lista de comandos. Véase: SpriteRenderer::SetCommandList()."));
	OSK_ASSERT(isStarted, InvalidObjectStateException("No se ha llamado a SpriteRenderer::Begin()."));

	// Optimización: comrpobamos si la imagen del sprite actual es la misma que la del sprite anterior.
	// Si es la misma, no necesitamos volver a establecer el material slot.
	// De lo contrario, debemos establecerlo.
	if (sprite.GetView() != previousImage) {
		targetCommandList->BindMaterialSlot(*sprite.GetTextureSlot());
		previousImage = sprite.GetView();
	}

	PushConst2D pushConst{};

	pushConst.color = sprite.color;
	pushConst.matrix = currentCameraMatrix * transform.GetAsMatrix();
	pushConst.texCoords = texCoords.type == TextureCoordsType::NORMALIZED
		? texCoords.texCoords
		: texCoords.GetNormalized(sprite.GetView()->GetSize2D().ToVector2f());

	targetCommandList->PushMaterialConstants("sprite", pushConst);

	targetCommandList->DrawSingleInstance(6);
}

void SpriteRenderer::Draw(const Sprite& sprite, const TextureCoordinates2D& texCoords, const Vector2f position, const Vector2f size, float rotation, const Color& color) {
	Transform2D transform(EMPTY_GAME_OBJECT);
	transform.SetPosition(position);
	transform.SetRotation(rotation);
	transform.SetScale(size);

	Draw(sprite, texCoords, transform, color);
}

void SpriteRenderer::Draw(const Sprite& sprite, const TextureCoordinates2D& texCoords, const ECS::Transform2D& transform) {
	Draw(sprite, texCoords, transform, sprite.color);
}

void SpriteRenderer::Draw(const Sprite& sprite, const ECS::Transform2D& transform) {
	Draw(sprite, transform, sprite.color);
}

void SpriteRenderer::Draw(const Sprite& sprite, const TextureCoordinates2D& texCoords, const Vector2f position, const Vector2f size, float rotation) {
	Draw(sprite, texCoords, position, size, rotation, sprite.color);
}

void SpriteRenderer::DrawString(Font& font, USize32 fontSize, std::string_view text, const ECS::Transform2D& transform, const Color& color) {
	const auto& fontInstance = font.GetInstance(fontSize);

	OSK_ASSERT(fontInstance.sprite.GetPointer() != nullptr, InvalidArgumentException("La fuente no tiene configurado su sprite."));

	const Vector2f& position = transform.GetPosition();
	float x = transform.GetPosition().x;
	float y = transform.GetPosition().y;
	
	for (UIndex32 i = 0; i < text.size(); i++) {
		const FontCharacter& character = fontInstance.characters.at(text[i]);

		if (text[i] == '\n') {
			y += character.size.y + character.bearing.y;
			x = position.x;

			continue;
		}

		if (text[i] == '\t') {
			x += (character.advance >> 6) * 4;// Font::SPACES_PER_TAB;

			continue;
		}

		if (text[i] == ' ') {
			x += (character.advance >> 6);

			continue;
		}

		float sizeX = character.size.x;
		float sizeY = character.size.y;

		float posX = x + character.bearing.x;
		float posY = y - (character.bearing.y);

		Draw(
			*fontInstance.sprite.GetPointer(), 
			TextureCoordinates2D::Pixels(character.texCoords.ToVector4f()), 
			Vector2f(posX, posY), 
			Vector2f(sizeX, sizeY) * transform.GetScale(), 
			transform.GetRotation(), color);

		x += static_cast<float>(character.advance >> 6);
	}
}

void SpriteRenderer::DrawString(ASSETS::Font& font, USize32 fontSize, std::string_view text, const Vector2f position, const Vector2f size, float rotation, const Color& color) {
	Transform2D transform(EMPTY_GAME_OBJECT);
	transform.SetPosition(position);
	transform.SetRotation(rotation);
	transform.SetScale(size);

	DrawString(font, fontSize, text, transform, color);
}

void SpriteRenderer::End() noexcept {
	currentlyBoundMaterial = nullptr;
	previousImage = nullptr;

	isStarted = false;
}
