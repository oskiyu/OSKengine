#include "Checkbox.h"

namespace OSK::UI {

	Checkbox::Checkbox(const Vector4& rectangle) : BaseUIElement(rectangle), IUserInterfaceMouseInput(this) {
		Rectangle = &this->rectangle;
	}


	Checkbox::~Checkbox() {

	}

	void Checkbox::Draw(SpriteBatch& spriteBatch) {
		IUserInterfaceImage::Draw(spriteBatch, *this);
	}

	void Checkbox::leftClick() {
		if (LinkedValue != nullptr)
			*LinkedValue = !*LinkedValue;

		IUserInterfaceMouseInput::leftClick();
	}

}
