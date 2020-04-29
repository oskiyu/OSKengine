#include "IUserInterfaceMouseInput.h"

#include "IUserInterfaceHasChildren.h"

namespace OSK::UI {

	IUserInterfaceMouseInput::IUserInterfaceMouseInput(BaseUIElement* base) {
		this->base = base;
	}


	bool IUserInterfaceMouseInput::IsBeingHovered() const {
		return isBeingHovered;
	}


	bool IUserInterfaceMouseInput::IsBeingLeftClicked() const {
		return isBeingLeftClicked;
	}


	bool IUserInterfaceMouseInput::IsBeingRightClicked() const {
		return isBeingRightClicked;
	}


	bool IUserInterfaceMouseInput::IsBeingWheelClicked() const {
		return isBeingWheelClicked;
	}


	void IUserInterfaceMouseInput::leftClick() {
		OnLeftClick();
	}

	void IUserInterfaceMouseInput::rightClick() {
		OnRightClick();
	}

	void IUserInterfaceMouseInput::mouseHover() {
		OnMouseHover();
	}

	void IUserInterfaceMouseInput::mouseUnhover() {
		OnMouseUnhover();
	}

	void IUserInterfaceMouseInput::wheelClick() {
		OnWheelClick();
	}

	void IUserInterfaceMouseInput::wheelChange(const float_t& val) {
		OnWheelChange(val);
	}


	void IUserInterfaceMouseInput::Update(const MouseState& mouseState) {
		if (base == nullptr)
			return;

		if (mouseState.GetMouseRectangle().Intersects(base->GetRectangle())) {
			if (!isBeingHovered)
				mouseHover();

			isBeingHovered = true;

			if (mouseState.IsButtonDown(ButtonCode::BUTTON_LEFT)) {
				if (!isBeingLeftClicked)
					leftClick();

				isBeingLeftClicked = true;
			}
			else {
				isBeingLeftClicked = false;
			}

			if (mouseState.IsButtonDown(ButtonCode::BUTTON_RIGHT)) {
				if (!isBeingRightClicked)
					rightClick();

				isBeingRightClicked = true;
			}
			else {
				isBeingRightClicked = false;
			}

			if (mouseState.IsButtonDown(ButtonCode::BUTTON_MIDDLE)) {
				if (!isBeingWheelClicked)
					wheelClick();

				isBeingWheelClicked = true;
			}
			else {
				isBeingWheelClicked = false;
			}

			if (mouseState.ScrollY != mouseState.OldScrollY) {
				wheelChange(mouseState.ScrollY - mouseState.OldScrollY);
			}
		}
		else {
			if (isBeingHovered)
				mouseUnhover();

			isBeingHovered = false;
		}

	}

}