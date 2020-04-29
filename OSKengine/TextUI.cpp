#include "TextUI.h"

namespace OSK::UI {

	Text::Text(const Vector4& rectangle, const std::string& text) : BaseUIElement(rectangle) {
		Texto = text;
	}

	
	Text::~Text() {

	}


	void Text::Draw(const RenderAPI& renderer) {
		IUserInterfaceText::Draw(renderer, *this);
	}

}
