#include "IUserInterfaceImage.h"

namespace OSK::UI {

	void IUserInterfaceImage::Draw(SpriteBatch& spriteBatch, const BaseUIElement& base) {
		spriteBatch.DrawSprite(ImageSprite);
	}

	void IUserInterfaceImage::UpdatePosition() {
		ImageSprite.SpriteTransform.SetPosition(Rectangle->GetRectanglePosition());
		ImageSprite.SpriteTransform.SetScale(Rectangle->GetRectangleSize());
	}

	void IUserInterfaceImage::SetSprite(Sprite sprite) {
		ImageSprite = sprite;

		ImageSprite.SpriteTransform.SetPosition(Rectangle->GetRectanglePosition());
		ImageSprite.SpriteTransform.SetScale(Rectangle->GetRectangleSize());
	}

}