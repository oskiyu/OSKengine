#include "BaseUIElement.h"

#include "RenderAPI.h"
#include "ToString.h"

namespace OSK::UI {

	BaseUIElement::BaseUIElement(const Vector4& rectangle) {
		SetRectangle(rectangle);
	}


	void BaseUIElement::SetRectangle(const Vector4& rectangle) {
		this->rectangle = rectangle;
		originalRectangle = rectangle;

		if (Parent != nullptr)
			SetPositionRelativeTo(Parent);
	}


	Vector4 BaseUIElement::GetRectangle() const {
		return rectangle;
	}


	void BaseUIElement::SetPositionRelativeTo(BaseUIElement* element) {
		if (element == nullptr)
			return;

		switch (ElementAnchor) {
			case Anchor::BOTTOM_LEFT:
				rectangle.X = originalRectangle.X + element->rectangle.X;
				rectangle.Y = originalRectangle.Y + element->rectangle.Y;
				break;

			case Anchor::BOTTOM_RIGHT:
				rectangle.X = element->rectangle.X + element->rectangle.GetRectangleWidth() - originalRectangle.X - originalRectangle.GetRectangleWidth();
				rectangle.Y = originalRectangle.Y + element->rectangle.Y;
				break;

			case Anchor::TOP_LEFT:
				rectangle.X = originalRectangle.X + element->rectangle.X;
				rectangle.Y = element->rectangle.Y + element->rectangle.GetRectangleHeight() - originalRectangle.Y - originalRectangle.GetRectangleHeight();
				break;

			case Anchor::TOP_RIGHT:
				rectangle.X = element->rectangle.X + element->rectangle.GetRectangleWidth() - originalRectangle.X - originalRectangle.GetRectangleWidth();
				rectangle.Y = element->rectangle.Y + element->rectangle.GetRectangleHeight() - originalRectangle.Y - originalRectangle.GetRectangleHeight();
				break;

			case Anchor::CENTER:
				rectangle.X = element->rectangle.X + element->rectangle.GetRectangleWidth() / 2 - originalRectangle.X - originalRectangle.GetRectangleWidth() / 2;
				rectangle.Y = element->rectangle.Y + element->rectangle.GetRectangleHeight() / 2 - originalRectangle.Y - originalRectangle.GetRectangleHeight() / 2;
				break;

			case Anchor::CENTER_TOP:
				rectangle.X = element->rectangle.X + element->rectangle.GetRectangleWidth() / 2 - originalRectangle.X - originalRectangle.GetRectangleWidth() / 2;
				rectangle.Y = element->rectangle.Y + element->rectangle.GetRectangleHeight() - originalRectangle.Y - originalRectangle.GetRectangleHeight();
				break;

			case Anchor::CENTER_BOTTOM:
				rectangle.X = element->rectangle.X + element->rectangle.GetRectangleWidth() / 2 - originalRectangle.X - originalRectangle.GetRectangleWidth() / 2;
				rectangle.Y = originalRectangle.Y + element->rectangle.Y;
				break;

			case Anchor::CENTER_LEFT:
				rectangle.X = originalRectangle.X + element->rectangle.X;
				rectangle.Y = element->rectangle.Y + element->rectangle.GetRectangleHeight() / 2 - originalRectangle.Y - originalRectangle.GetRectangleHeight() / 2;
				break;

			case Anchor::CENTER_RIGHT:
				rectangle.X = element->rectangle.X + element->rectangle.GetRectangleWidth() - originalRectangle.X - originalRectangle.GetRectangleWidth();
				rectangle.Y = element->rectangle.Y + element->rectangle.GetRectangleHeight() / 2 - originalRectangle.Y - originalRectangle.GetRectangleHeight() / 2;
				break;
		}
	}
}

