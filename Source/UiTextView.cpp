#include "UiTextView.h"

#include "SpriteRenderer.h"
#include "Transform2D.h"

#include "Font.h"
#include "FontInstance.h"

using namespace OSK;
using namespace OSK::UI;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void TextView::AdjustSizeToText() {
	if (!font.GetAsset())
		return;

	const FontInstance& fontInstance = font->GetInstance(fontSize);
	const FontCharacter& referenceChar = fontInstance.characters.at('A');


	float totalSizeX = 0.0f;
	float totalSizeY = 0.0f;

	float currentSizeX = 0.0f;
	float currentLineStartY = 0.0f;

	float currentLineTop = 0.0f;
	float currentLineBottom = 0.0f;

	for (const char c : text) {
		if (c == '\n') {
			totalSizeX = glm::max(totalSizeX, currentSizeX);

			currentSizeX = 0.0f;
			currentLineStartY = totalSizeY;
			totalSizeY += referenceChar.size.y + referenceChar.bearing.y;

			continue;
		}
		if (c == '\t') {
			currentSizeX += (fontInstance.characters.at(c).advance >> 6) * 4;

			continue;
		}

		const FontCharacter& character = fontInstance.characters.at(c);

		const float characterTop = -character.bearing.y;
		const float characterBottom = characterTop + character.size.y;
		
		currentLineTop = glm::min(currentLineTop, characterTop);
		currentLineBottom = glm::max(currentLineBottom, characterBottom);

		currentSizeX += character.advance >> 6;
	}

	totalSizeX = glm::max(totalSizeX, currentSizeX);
	totalSizeY = glm::max(totalSizeY, currentLineStartY + (currentLineBottom - currentLineTop));

	const Vector2f newSize = Vector2f(
		totalSizeX,
		totalSizeY
	);

	size = newSize + Vector2f(
		GetPadding().x + GetPadding().Z,
		GetPadding().y + GetPadding().W
	);
}

void TextView::SetFontSize(USize32 size) {
	fontSize = size;
	font->LoadSizedFont(size);
}

void TextView::SetFont(ASSETS::AssetRef<ASSETS::Font> font) {
	this->font = font;
}

void TextView::SetText(const std::string& text) {
	this->text = text;
}

const ASSETS::Font* TextView::GetFont() const {
	return font.GetAsset();
}

USize32 TextView::GetFontSize() const {
	return fontSize;
}

std::string_view TextView::GetText() const {
	return text;
}

void TextView::Render(SpriteRenderer* renderer, Vector2f parentPosition) const {
	if (!font.GetAsset())
		return;

	ImageView::Render(renderer, parentPosition);
	
	Vector2f globalPosition = GetContentTopLeftPosition() + parentPosition;
	globalPosition.y += GetContentSize().y;
	globalPosition = globalPosition.ToVector2i().ToVector2f();

	Transform2D transform(EMPTY_GAME_OBJECT);
	transform.SetPosition(globalPosition);

	renderer->DrawString(*font.GetAsset(), fontSize, text, transform, Color::White);
}
