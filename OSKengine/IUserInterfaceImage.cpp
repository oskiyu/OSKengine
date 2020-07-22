#include "IUserInterfaceImage.h"

namespace OSK::UI {

	void IUserInterfaceImage::Draw(const RenderAPI& renderer, const BaseUIElement& base) {
		if (ImageTexture != nullptr)
			renderer.DrawTexture(*ImageTexture, base.GetRectangle().GetRectanglePosition(), base.GetRectangle().GetRectangleSize(), ImageColor * base.Opacity);
	}

	void IUserInterfaceImage::Draw(SpriteBatch& spriteBatch, const BaseUIElement& base) {
		spriteBatch.DrawSprite(Image);
	}

	void IUserInterfaceImage::SetSprite(Sprite sprite) {
		Image = sprite;


		Image.SpriteTransform.SetPosition(Rectangle->GetRectanglePosition());
		Image.SpriteTransform.SetScale(Rectangle->GetRectangleSize());
	}

}