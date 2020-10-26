#include "IUserInterfaceText.h"

namespace OSK::UI {

	void IUserInterfaceText::Draw(SpriteBatch& spriteBatch, const BaseUIElement& base) {
		if (LinkedText != nullptr)
			spriteBatch.DrawString(TextFont, *LinkedText, TextSize, Vector2(0.0f), TextColor * base.Opacity, TextAnchor, Vector4(base.GetRectangle().X + TextBorderLimit, base.GetRectangle().Y + TextBorderLimit, base.GetRectangle().Z - TextBorderLimit, base.GetRectangle().W - TextBorderLimit), TextRenderingLimit::NEW_LINE);
		else
			spriteBatch.DrawString(TextFont, Texto, TextSize, Vector2(0.0f), TextColor * base.Opacity, TextAnchor, Vector4(base.GetRectangle().X + TextBorderLimit, base.GetRectangle().Y + TextBorderLimit, base.GetRectangle().Z - TextBorderLimit, base.GetRectangle().W - TextBorderLimit), TextRenderingLimit::NEW_LINE);
	}

}