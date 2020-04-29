#include "IUserInterfaceImage.h"

namespace OSK::UI {

	void IUserInterfaceImage::Draw(const RenderAPI& renderer, const BaseUIElement& base) {
		if (ImageTexture != nullptr)
			renderer.DrawTexture(*ImageTexture, base.GetRectangle().GetRectanglePosition(), base.GetRectangle().GetRectangleSize(), ImageColor * base.Opacity);
	}

}