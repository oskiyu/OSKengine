#include "UiButton.h"

using namespace OSK;
using namespace OSK::UI;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


Button::Button(const Vector2f size) : Button(size, "") {

}

Button::Button(const Vector2f size, const std::string& text) 
	: IElement(size), defaultImage(size), selectedImage(size), pressedImage(size), buttonText(size) {
	buttonText.SetAnchor(Anchor::CENTER_X | Anchor::CENTER_Y);

	buttonText.SetText(text);
	buttonText.AdjustSizeToText();
}

void Button::SetSize(Vector2f size) {
	IElement::SetSize(size);

	defaultImage.SetSize(size);
	selectedImage.SetSize(size);
	pressedImage.SetSize(size);
}

void Button::Render(SpriteRenderer* renderer, Vector2f parentPosition) const {
	if (!IsVisible())
		return;

	GetCurrentImage().Render(renderer, parentPosition);
	buttonText.Render(renderer, parentPosition);
}

void Button::Click() {
	if (type == Type::TOGGLE) {
		currentToggleState = !currentToggleState;
		callback(currentToggleState);
	}
	else {
		callback(true);
	}
}

bool Button::UpdateByCursor(Vector2f cursor, bool isPressed, Vector2f parentPosition) {
	if (IsLocked() || !IsVisible())
		return false;

	const bool isOverButton = GetRectangle(parentPosition).ContainsPoint(cursor);

	if (!isOverButton) {
		if (type == Type::TOGGLE) {
			currentState = currentToggleState
				? State::PRESSED
				: State::DEFAULT;
		}
		else {
			currentState = State::DEFAULT;
		}

		return false;
	}

	currentState = isPressed
		? State::PRESSED
		: State::SELECTED;
	
	if (isPressed && !wasPreviousFramePressed)
		Click();

	wasPreviousFramePressed = isPressed;

	return true;
}

void Button::SetCallback(CallbackFnc callback) {
	this->callback = callback;
}

void Button::SetState(State state) {
	currentState = state;
	currentToggleState = currentState == State::PRESSED;
}

void Button::SetType(Type type) {
	this->type = type;
}

Button::Type Button::GetType() const {
	return type;
}

void Button::SetTextFont(Font* font) {
	buttonText.SetFont(font);
	buttonText.AdjustSizeToText();
}

void Button::SetTextFontSize(USize32 fontSize) {
	buttonText.SetFontSize(fontSize);
	buttonText.AdjustSizeToText();
}

Sprite& Button::GetSprite(State state) {
	switch (state) {
	case Button::State::DEFAULT:
		return defaultImage.GetSprite();
	case Button::State::SELECTED:
		return selectedImage.GetSprite();
	case Button::State::PRESSED:
		return pressedImage.GetSprite();
	default:
		break;
	}
}

const Sprite& Button::GetSprite(State state) const {
	switch (state) {
	case Button::State::DEFAULT:
		return defaultImage.GetSprite();
	case Button::State::SELECTED:
		return selectedImage.GetSprite();
	case Button::State::PRESSED:
		return pressedImage.GetSprite();
	default:
		break;
	}
}

Sprite& Button::GetDefaultSprite() {
	return GetSprite(State::DEFAULT);
}

const Sprite& Button::GetDefaultSprite() const {
	return GetSprite(State::DEFAULT);
}

Sprite& Button::GetSelectedSprite() {
	return GetSprite(State::SELECTED);
}

const Sprite& Button::GetSelectedSprite() const {
	return GetSprite(State::SELECTED);
}

Sprite& Button::GetPressedSprite() {
	return GetSprite(State::PRESSED);
}

const Sprite& Button::GetPressedSprite() const {
	return GetSprite(State::PRESSED);
}

const ImageView& Button::GetCurrentImage() const {
	switch (currentState) {
	case Button::State::DEFAULT:
		return defaultImage;
	case Button::State::SELECTED:
		return selectedImage;
	case Button::State::PRESSED:
		return pressedImage;
	default:
		break;
	}
}

void Button::SetText(const std::string& text) {
	buttonText.SetText(text);
	buttonText.AdjustSizeToText();
}

std::string_view Button::GetText() const {
	return buttonText.GetText();
}

void Button::_SetRelativePosition(const Vector2f& relativePosition) {
	defaultImage._SetRelativePosition(relativePosition);
	selectedImage._SetRelativePosition(relativePosition);
	pressedImage._SetRelativePosition(relativePosition);

	buttonText._SetRelativePosition(relativePosition + GetTextRelativePosition());

	IElement::_SetRelativePosition(relativePosition);
}

Vector2f Button::GetTextRelativePosition() const {
	return size * 0.5f - buttonText.GetSize() * 0.5f;
}
