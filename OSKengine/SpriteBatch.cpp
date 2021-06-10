#include "SpriteBatch.h"

#include "VulkanRenderer.h"
#include "AnchorTextToEnum.h"
#include "Camera2D.h"
#include "SpriteContainer.h"

namespace OSK {

	SpriteBatch::SpriteBatch() {

	}

	SpriteBatch::~SpriteBatch() {
		spritesToDraw.Free();
	}

	void SpriteBatch::DrawSprite(Sprite sprite) {
		OSK_ASSERT(camera != nullptr, "La cámara es null.");

		SpriteContainer spr;
		spr.Set(sprite, camera->GetProjection());

		spritesToDraw.Insert(spr);
	}

	void SpriteBatch::DrawString(const Font* fuente, const std::string& texto, float size, const Vector2& position, const Color& color, Anchor screenAnchor, const Vector4& reference, TextRenderingLimit limitAction, float sizeXlimit, float limitOffset) {
		OSK_ASSERT(camera, "La cámara es null.");

		AnchorTextTo to = AnchorTextTo::SCREEN;
		if (reference.X > 0.0f)
			to = AnchorTextTo::UI_ELEMENT;

		Vector2 textSize = fuente->GetTextSize(texto, Vector2(size));
		Vector2 finalPosition = GetTextPosition(position, textSize, screenAnchor, to, reference);

		std::string finalText = texto;

		if (limitAction == OSK::TextRenderingLimit::MOVE_TEXT) {
			if (finalPosition.X + textSize.X + limitOffset > renderer->defaultCamera2D.targetSize.X)
				finalPosition.X -= limitOffset + (finalPosition.X + textSize.X - renderer->defaultCamera2D.targetSize.X);
		}

		else if (limitAction == TextRenderingLimit::NEW_LINE) {
			float_t textSizeX = 0.0f;

			for (auto c = texto.begin(); c != texto.end(); c++) {
				FontChar character = fuente->characters.at(*c);
				if (*c == '\n') {
					textSizeX = 0;
				}
				else if (*c == '\t') {
					textSizeX += (character.bearing.X * size + character.size.X * size) * 3;
				}
				else {
					textSizeX += character.bearing.X * size + character.size.X * size;
				}

				if (textSizeX + finalPosition.X + limitOffset > renderer->window->GetSize().X) {
					finalText.insert(c - 1, '\n');
					textSize = 0;
				}
			}
		}

		//Render:
		float_t x = finalPosition.X;
		float_t y = finalPosition.Y + fuente->characters.at('A').size.Y * size;
		uint32_t it = 0;
		for (auto c = finalText.begin(); c != finalText.end(); c++) {

			FontChar character = fuente->characters.at(*c);
			if (*c == '\n') {
				y += character.size.Y * size + character.bearing.Y;
				x = finalPosition.X;

				continue;
			}

			if (*c == '\t') {
				x += x += (character.advance >> 6) * size * Font::SPACES_PER_TAB;

				continue;
			}

			if (*c == ' ') {
				x += (character.advance >> 6) * size;

				continue;
			}

			float sizeX = character.size.X * size;
			float sizeY = character.size.Y * size;

			float posX = x + character.bearing.X * size;
			float posY = y - (character.bearing.Y) * size;
			
			bool isOutOfScreen = posX > renderer->window->GetSize().X || posY > renderer->window->GetSize().Y
				|| posY + sizeY < 0 || posX + sizeX < 0;

			if (isOutOfScreen) {
				it++;
				x += ((character.advance >> 6) * size);

				continue;
			}

			character.sprite.transform.SetPosition(Vector2i((int)posX, (int)posY).ToVector2f());
			character.sprite.transform.SetScale(Vector2i((int)sizeX, (int)sizeY).ToVector2f());
			character.sprite.color = color;

			it++;

			SpriteContainer spr;
			spr.Set(character.sprite, camera->GetProjection());

			spritesToDraw.Insert(spr);

			x += ((character.advance >> 6) * size); // character.Bearing.X;//((character.Advance * (int)sizeX) >> 6) * size;
		}
	}

	Vector2 SpriteBatch::GetTextPosition(const Vector2& position, const Vector2& textSize, const Anchor& anchor, const AnchorTextTo& to, const Vector4& reference) const {
		Vector4 refr = Vector4(0.0f, 0.0f, renderer->defaultCamera2D.targetSize.X, renderer->defaultCamera2D.targetSize.Y);

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

	void SpriteBatch::SetCamera(Camera2D* camera) {
		this->camera = camera;
	}

	void SpriteBatch::Clear() {
		spritesToDraw.Empty();
	}

}
