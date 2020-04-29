#include "Checkbox.h"

namespace OSK::UI {

	Checkbox::Checkbox(const Vector4& rectangle) : BaseUIElement(rectangle), IUserInterfaceMouseInput(this) {

	}


	Checkbox::~Checkbox() {

	}


	void Checkbox::Draw(const RenderAPI& renderer) {
		IUserInterfaceImage::Draw(renderer, *this);
	}


	void Checkbox::leftClick() {
		if (LinkedValue != nullptr)
			*LinkedValue = !*LinkedValue;

		IUserInterfaceMouseInput::leftClick();
	}

}
