#include "IUserInterfaceText.h"

namespace OSK::UI {

	void IUserInterfaceText::Draw(const RenderAPI& renderer, const BaseUIElement& base) {
		if (TextFont == nullptr)
			return;

		if (LinkedText != nullptr)
			renderer.DrawString(*TextFont, TextSize, *LinkedText, Vector2(0.0f), TextColor * base.Opacity, TextAnchor, Vector4(base.GetRectangle().X + TextBorderLimit, base.GetRectangle().Y + TextBorderLimit, base.GetRectangle().Z - TextBorderLimit, base.GetRectangle().W - TextBorderLimit), TextRenderingLimit::NEW_LINE);
		else	
			renderer.DrawString(*TextFont, TextSize, Texto, Vector2(0.0f), TextColor * base.Opacity, TextAnchor, Vector4(base.GetRectangle().X + TextBorderLimit, base.GetRectangle().Y + TextBorderLimit, base.GetRectangle().Z - TextBorderLimit, base.GetRectangle().W - TextBorderLimit), TextRenderingLimit::NEW_LINE);
	}

}