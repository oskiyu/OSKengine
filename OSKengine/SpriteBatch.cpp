#include "SpriteBatch.h"

#include "VulkanRenderer.h"
#include "AnchorTextToEnum.h"

namespace OSK {

	SpriteBatch::SpriteBatch() {

	}

	void SpriteBatch::DrawSprite(Sprite sprite) {
		spritesToDraw.push_front(sprite);
	}

	void SpriteBatch::DrawString(const Font* fuente, const std::string& texto, const float_t& size, const Vector2& position, const Color& color, const Anchor& screenAnchor, const Vector4& reference, const TextRenderingLimit& limitAction, const float_t& sizeXlimit, const float_t& limitOffset) {
		AnchorTextTo to = AnchorTextTo::SCREEN;
		if (reference.X > 0.0f)
			to = AnchorTextTo::UI_ELEMENT;

		Vector2 textSize = fuente->GetTextSize(texto, Vector2(size));
		Vector2 finalPosition = GetTextPosition(position, textSize, screenAnchor, to, reference);

		std::string finalText = texto;

		if (limitAction == OSK::TextRenderingLimit::MOVE_TEXT) {
			if (finalPosition.X + textSize.X + limitOffset > renderer->DefaultCamera2D.TargetSize.X)
				finalPosition.X -= limitOffset + (finalPosition.X + textSize.X - renderer->DefaultCamera2D.TargetSize.X);
		}

		else if (limitAction == TextRenderingLimit::NEW_LINE) {
			float_t textSizeX = 0.0f;

			for (auto c = texto.begin(); c != texto.end(); c++) {
				FontChar character = fuente->Characters.at(*c);
				if (*c == '\n') {
					textSizeX = 0;
				}
				else if (*c == '\t') {
					textSizeX += (character.Bearing.X * size + character.Size.X * size) * 3;
				}
				else {
					textSizeX += character.Bearing.X * size + character.Size.X * size;
				}

				if (textSizeX + finalPosition.X + limitOffset > renderer->Window->ScreenSizeX) {
					finalText.insert(c - 1, '\n');
					textSize = 0;
				}
			}
		}

		//Render:
		float_t x = finalPosition.X;
		float_t y = finalPosition.Y + fuente->Characters.at('A').Size.Y * size;
		for (auto c = finalText.begin(); c != finalText.end(); c++) {

			FontChar character = fuente->Characters.at(*c);
			if (*c == '\n') {
				y += character.Size.Y * size + character.Bearing.Y;
				x = finalPosition.X;

				continue;
			}

			if (*c == '\t') {
				x += x += (character.Advance >> 6) * size * Font::SPACES_PER_TAB;

				continue;
			}

			if (*c == ' ') {
				x += (character.Advance >> 6) * size;

				continue;
			}

			float sizeX = character.Size.X * size;
			float sizeY = character.Size.Y * size;

			float posX = x + character.Bearing.X * size;
			float posY = y - (character.Bearing.Y) * size;

			character.sprite.SpriteTransform.SetPosition(Vector2((int)posX, (int)posY));
			character.sprite.SpriteTransform.SetScale(Vector2((int)sizeX, (int)sizeY));
			character.sprite.color = color;

			DrawSprite(character.sprite);

			x += ((character.Advance >> 6) * size); // character.Bearing.X;//((character.Advance * (int)sizeX) >> 6) * size;
		}
	}

	Vector2 SpriteBatch::GetTextPosition(const Vector2& position, const Vector2& textSize, const Anchor& anchor, const AnchorTextTo& to, const Vector4& reference) const {
		Vector4 refr = Vector4(0.0f, 0.0f, renderer->DefaultCamera2D.TargetSize.X, renderer->DefaultCamera2D.TargetSize.Y);

		if (to == AnchorTextTo::UI_ELEMENT)
			refr = reference;

		switch (anchor) {
		case Anchor::TOP_LEFT:
			return Vector2(refr.X + position.X, refr.Y + position.Y);

		case Anchor::TOP_RIGHT:
			return Vector2(refr.X + position.X + refr.GetRectangleWidth() - textSize.X, refr.Y + position.Y);

		case Anchor::BOTTOM_LEFT:
			return Vector2(refr.X + position.X, refr.Y - position.Y + refr.GetRectangleHeight() - textSize.Y);

		case Anchor::BOTTOM_RIGHT:
			return Vector2(refr.X + position.X + refr.GetRectangleWidth() - textSize.X, refr.Y - position.Y + refr.GetRectangleHeight() - textSize.Y);

		case Anchor::CENTER:
			return Vector2(refr.X + position.X + refr.GetRectangleWidth() / 2 - textSize.X / 2, refr.Y + position.Y + refr.GetRectangleHeight() / 2 - textSize.Y / 2);

		default:
			return Vector2(refr.X + position.X, refr.Y + position.Y);
		}
	}

	void SpriteBatch::Clear() {
		spritesToDraw.clear();
	}

}
