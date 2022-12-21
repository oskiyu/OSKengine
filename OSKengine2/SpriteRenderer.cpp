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

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void SpriteRenderer::SetCommandList(ICommandList* commandList) noexcept {
	targetCommandList = commandList;
}

void SpriteRenderer::Begin() {
	OSK_ASSERT(lastBoundMaterial == nullptr, "Se debe llamar antes a End.");
	OSK_ASSERT(!isStarted, "Se debe llamar antes a End.");
	OSK_ASSERT(targetCommandList, "No se ha establecido la lista de comandos. Véase: SpriteRenderer::SetCommandList().");

	targetCommandList->BindVertexBuffer(Sprite::globalVertexBuffer);
	targetCommandList->BindIndexBuffer(Sprite::globalIndexBuffer);

	isStarted = true;
}

void SpriteRenderer::Draw(const GRAPHICS::Sprite& sprite, const ECS::Transform2D& transform, const Color& color) {
	Draw(sprite, sprite.GetTexCoords(), transform, color);
}

void SpriteRenderer::Draw(const Sprite& sprite, const TextureCoordinates2D& texCoords, const ECS::Transform2D& transform, const Color& color) {
	OSK_ASSERT(targetCommandList, "No se ha establecido la lista de comandos. Véase: SpriteRenderer::SetCommandList().");
	OSK_ASSERT(isStarted, "No se ha llamado a SpriteRenderer::Begin().");

	bool rebind = false;

	PushConst2D pushConst{};

	pushConst.color = sprite.color;
	pushConst.matrix = transform.GetAsMatrix();

	if (texCoords.type == TextureCoordsType::NORMALIZED)
		pushConst.texCoords = texCoords.texCoords;
	else
		pushConst.texCoords = texCoords.GetNormalized({ (float)sprite.GetGpuImage()->GetSize().X, (float)sprite.GetGpuImage()->GetSize().Y });

	if (lastBoundMaterial != sprite.GetMaterialInstance()->GetMaterial()) {
		targetCommandList->BindMaterial(sprite.GetMaterialInstance()->GetMaterial());
		lastBoundMaterial = sprite.GetMaterialInstance()->GetMaterial();

		rebind = true;
	}

	targetCommandList->PushMaterialConstants("sprite", pushConst);

	if (previousMaterialInstance != sprite.GetMaterialInstance()) {
		previousMaterialInstance = sprite.GetMaterialInstance();
		rebind = true;
	}

	if (rebind)
		for (const auto& [slotName, slot] : previousMaterialInstance->GetLayout()->GetAllSlots())
			targetCommandList->BindMaterialSlot(previousMaterialInstance->GetSlot(slotName));

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

void SpriteRenderer::DrawString(ASSETS::Font& font, TSize fontSize, const std::string& text, const ECS::Transform2D& transform, const Color& color) {
	const auto& fontInstance = font.GetInstance(fontSize);

	OSK_ASSERT(fontInstance.sprite.GetPointer() != nullptr, "La fuente no tiene configurado su sprite.");
	OSK_ASSERT(fontInstance.sprite->GetMaterialInstance() != nullptr, "La fuente no tiene configurado su sprite.");

	const Vector2f& position = transform.GetPosition();
	float x = transform.GetPosition().X;
	float y = transform.GetPosition().Y;
	
	for (TSize i = 0; i < text.size(); i++) {
		const FontCharacter& character = fontInstance.characters.Get(text[i]);

		if (text[i] == '\n') {
			y += character.size.Y + character.bearing.Y;
			x = position.X;

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

		float sizeX = character.size.X;
		float sizeY = character.size.Y;

		float posX = x + character.bearing.X;
		float posY = y - (character.bearing.Y);

		Draw(*fontInstance.sprite.GetPointer(), TextureCoordinates2D::Pixels(character.texCoords.ToVector4f()), Vector2f(posX, posY), Vector2f(sizeX, sizeY) * transform.GetScale(), transform.GetRotation(), color);

		x += character.advance >> 6;
	}
}

void SpriteRenderer::DrawString(ASSETS::Font& font, TSize fontSize, const std::string& text, const Vector2f position, const Vector2f size, float rotation, const Color& color) {
	Transform2D transform(EMPTY_GAME_OBJECT);
	transform.SetPosition(position);
	transform.SetRotation(rotation);
	transform.SetScale(size);

	DrawString(font, fontSize, text, transform, color);
}

void SpriteRenderer::End() noexcept {
	lastBoundMaterial = nullptr;

	isStarted = false;
}
