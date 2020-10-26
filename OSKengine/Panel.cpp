#include "Panel.h"

namespace OSK::UI {

	Panel::Panel(const Vector4& rectangle) : BaseUIElement(rectangle), IUserInterfaceHasChildren(this) {
		Rectangle = &this->rectangle;
	}


	Panel::~Panel() {

	}


	void Panel::Draw(SpriteBatch& spriteBatch) {
		IUserInterfaceImage::Draw(spriteBatch, *this);
		IUserInterfaceHasChildren::Draw(spriteBatch);
	}


	void Panel::SetPositionRelativeTo(BaseUIElement* element) {
		BaseUIElement::SetPositionRelativeTo(element);

		IUserInterfaceHasChildren::UpdateChildrenPositions();
	}

}