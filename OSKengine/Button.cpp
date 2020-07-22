#include "Button.h"

namespace OSK::UI {

	Button::Button() : BaseUIElement(Vector4{ 0.0f }), IUserInterfaceMouseInput(this) {
		Rectangle = &this->rectangle;
	}


	Button::~Button() {

	}


	void Button::Draw(const RenderAPI& renderer) {
		IUserInterfaceImage::Draw(renderer, *this);
		IUserInterfaceText::Draw(renderer, *this);
	}


	void Button::Draw(SpriteBatch& spriteBatch) {
		IUserInterfaceImage::Draw(spriteBatch, *this);
		IUserInterfaceText::Draw(spriteBatch, *this);
	}

}