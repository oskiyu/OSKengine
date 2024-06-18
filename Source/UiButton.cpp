#include "UiButton.h"

using namespace OSK;
using namespace OSK::UI;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


Button::Button(const Vector2f size) : Button(size, "") {

}

Button::Button(const Vector2f size, const std::string& text) 
	: IElement(size), m_defaultImage(size), m_selectedImage(size), m_pressedImage(size), m_buttonText(size) {
	m_buttonText.SetAnchor(Anchor::CENTER_X | Anchor::CENTER_Y);

	m_buttonText.SetText(text);
	m_buttonText.AdjustSizeToText();
}

void Button::SetSize(Vector2f size) {
	IElement::SetSize(size);

	m_defaultImage.SetSize(size);
	m_selectedImage.SetSize(size);
	m_pressedImage.SetSize(size);
}

void Button::Render(SdfBindlessRenderer2D* renderer) const {
	IElement::Render(renderer);

	switch (m_currentState) {
	case Button::State::DEFAULT:
		m_defaultImage.Render(renderer);
		break;
	case Button::State::SELECTED:
		m_selectedImage.Render(renderer);
		break;
	case Button::State::PRESSED:
		m_pressedImage.Render(renderer);
		break;
	default:
		break;
	}

	m_buttonText.Render(renderer);
}

void Button::Click() {
	if (m_type == Type::TOGGLE) {
		m_currentToggleState = !m_currentToggleState;
		m_callback(m_currentToggleState);
	}
	else {
		m_callback(true);
	}
}

bool Button::UpdateByCursor(Vector2f cursor, bool isPressed) {
	if (IsLocked() || !IsVisible())
		return false;

	const bool isOverButton = GetRectangle().ContainsPoint(cursor);

	if (!isOverButton) {
		if (m_type == Type::TOGGLE) {
			m_currentState = m_currentToggleState
				? State::PRESSED
				: State::DEFAULT;
		}
		else {
			m_currentState = State::DEFAULT;
		}

		return false;
	}

	m_currentState = isPressed
		? State::PRESSED
		: State::SELECTED;
	
	if (isPressed && !m_wasPreviousFramePressed)
		Click();

	m_wasPreviousFramePressed = isPressed;

	return true;
}

void Button::SetCallback(CallbackFnc callback) {
	m_callback = callback;
}

void Button::SetState(State state) {
	m_currentState = state;
	m_currentToggleState = m_currentState == State::PRESSED;
}

void Button::SetType(Type type) {
	m_type = type;
}

Button::Type Button::GetType() const {
	return m_type;
}

void Button::SetTextFont(ASSETS::AssetRef<ASSETS::Font> font) {
	m_buttonText.SetFont(font);
	m_buttonText.AdjustSizeToText();
}

void Button::SetTextFontSize(USize32 fontSize) {
	m_buttonText.SetFontSize(fontSize);
	m_buttonText.AdjustSizeToText();
}

DynamicArray<GRAPHICS::SdfDrawCall2D>& Button::GetDrawCalls(State state) {
	switch (state) {
	case Button::State::DEFAULT:
		return m_defaultImage.GetAllDrawCalls();
	case Button::State::SELECTED:
		return m_selectedImage.GetAllDrawCalls();
	case Button::State::PRESSED:
		return m_pressedImage.GetAllDrawCalls();
	default:
		break;
	}
}

const DynamicArray<GRAPHICS::SdfDrawCall2D>& Button::GetDrawCalls(State state) const {
	switch (state) {
	case Button::State::DEFAULT:
		return m_defaultImage.GetAllDrawCalls();
	case Button::State::SELECTED:
		return m_selectedImage.GetAllDrawCalls();
	case Button::State::PRESSED:
		return m_pressedImage.GetAllDrawCalls();
	default:
		break;
	}
}

DynamicArray<GRAPHICS::SdfDrawCall2D>& Button::GetDefaultDrawCalls() {
	return GetDrawCalls(State::DEFAULT);
}

const DynamicArray<GRAPHICS::SdfDrawCall2D>& Button::GetDefaultDrawCalls() const {
	return GetDrawCalls(State::DEFAULT);
}

DynamicArray<GRAPHICS::SdfDrawCall2D>& Button::GetSelectedDrawCalls() {
	return GetDrawCalls(State::SELECTED);
}

const DynamicArray<GRAPHICS::SdfDrawCall2D>& Button::GetSelectedDrawCalls() const {
	return GetDrawCalls(State::SELECTED);
}

DynamicArray<GRAPHICS::SdfDrawCall2D>& Button::GetPressedDrawCalls() {
	return GetDrawCalls(State::PRESSED);
}

const DynamicArray<GRAPHICS::SdfDrawCall2D>& Button::GetPressedDrawCalls() const {
	return GetDrawCalls(State::PRESSED);
}

void Button::SetText(const std::string& text) {
	m_buttonText.SetText(text);
	m_buttonText.AdjustSizeToText();
}

std::string_view Button::GetText() const {
	return m_buttonText.GetText();
}

void Button::_SetPosition(const Vector2f& newPosition) {
	IElement::_SetPosition(newPosition);

	m_defaultImage._SetPosition(newPosition);
	m_selectedImage._SetPosition(newPosition);
	m_pressedImage._SetPosition(newPosition);

	m_buttonText._SetPosition(newPosition + GetTextRelativePosition());
}

Vector2f Button::GetTextRelativePosition() const {
	return GetSize() * 0.5f - m_buttonText.GetSize() * 0.5f;
}
