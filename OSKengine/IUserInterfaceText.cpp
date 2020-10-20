#include "IUserInterfaceText.h"

namespace OSK::UI {

	/*void IUserInterfaceText::Draw(const RenderAPI& renderer, const BaseUIElement& base) {
		if (OldTextFont == nullptr)
			return;

		if (LinkedText != nullptr)
			renderer.DrawString(*OldTextFont, TextSize, *LinkedText, Vector2(0.0f), TextColor * base.Opacity, TextAnchor, Vector4(base.GetRectangle().X + TextBorderLimit, base.GetRectangle().Y + TextBorderLimit, base.GetRectangle().Z - TextBorderLimit, base.GetRectangle().W - TextBorderLimit), TextRenderingLimit::NEW_LINE);
		else
			renderer.DrawString(*OldTextFont, TextSize, Texto, Vector2(0.0f), TextColor * base.Opacity, TextAnchor, Vector4(base.GetRectangle().X + TextBorderLimit, base.GetRectangle().Y + TextBorderLimit, base.GetRectangle().Z - TextBorderLimit, base.GetRectangle().W - TextBorderLimit), TextRenderingLimit::NEW_LINE);
	}
	*/
	void IUserInterfaceText::Draw(SpriteBatch& spriteBatch, const BaseUIElement& base) {
		if (LinkedText != nullptr)
			spriteBatch.DrawString(TextFont, *LinkedText, TextSize, Vector2(0.0f), TextColor * base.Opacity, TextAnchor, Vector4(base.GetRectangle().X + TextBorderLimit, base.GetRectangle().Y + TextBorderLimit, base.GetRectangle().Z - TextBorderLimit, base.GetRectangle().W - TextBorderLimit), TextRenderingLimit::NEW_LINE);
		else
			spriteBatch.DrawString(TextFont, Texto, TextSize, Vector2(0.0f), TextColor * base.Opacity, TextAnchor, Vector4(base.GetRectangle().X + TextBorderLimit, base.GetRectangle().Y + TextBorderLimit, base.GetRectangle().Z - TextBorderLimit, base.GetRectangle().W - TextBorderLimit), TextRenderingLimit::NEW_LINE);
	}

}