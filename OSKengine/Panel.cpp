#include "Panel.h"

namespace OSK::UI {

	Panel::Panel(const Vector4& rectangle) : BaseUIElement(rectangle), IUserInterfaceHasChildren(this) {

	}


	Panel::~Panel() {

	}


	void Panel::Draw(const RenderAPI& renderer) {
		IUserInterfaceImage::Draw(renderer, *this);
		IUserInterfaceHasChildren::Draw(renderer);
	}


	void Panel::SetPositionRelativeTo(BaseUIElement* element) {
		BaseUIElement::SetPositionRelativeTo(element);

		IUserInterfaceHasChildren::UpdateChildrenPositions();
	}

}