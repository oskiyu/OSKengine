#include "IUserInterfaceImage.h"

namespace OSK::UI {

	void IUserInterfaceImage::Draw(SpriteBatch& spriteBatch, const BaseUIElement& base) {
		spriteBatch.DrawSprite(Image);
	}

	void IUserInterfaceImage::UpdatePosition() {
		Image.SpriteTransform.SetPosition(Rectangle->GetRectanglePosition());
		Image.SpriteTransform.SetScale(Rectangle->GetRectangleSize());
	}

	void IUserInterfaceImage::SetSprite(Sprite sprite) {
		Image = sprite;

		Image.SpriteTransform.SetPosition(Rectangle->GetRectanglePosition());
		Image.SpriteTransform.SetScale(Rectangle->GetRectangleSize());
	}

}