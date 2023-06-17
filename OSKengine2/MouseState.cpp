#include "MouseState.h"

#include <memory>

using namespace OSK;
using namespace OSK::IO;

MouseState::MouseState() {
	memset(buttonStates, 0, sizeof(buttonStates));
}

ButtonState MouseState::GetButtonState(MouseButton button) const {
	return buttonStates[(int)button];
}

bool MouseState::IsButtonDown(MouseButton button) const {
	return GetButtonState(button) == ButtonState::PRESSED;
}

bool MouseState::IsButtonUp(MouseButton button) const {
	return !IsButtonDown(button);
}

Vector2i MouseState::GetPosition() const {
	return position;
}

Vector2f MouseState::GetRelativePosition() const {
	return relativePosition;
}

int MouseState::GetScroll() const {
	return scroll.y;
}

int MouseState::GetHorizontalScroll() const {
	return scroll.x;
}

void MouseState::_SetScrollX(int x) {
	scroll.x = x;
}
void MouseState::_SetScrollY(int y) {
	scroll.y = y;
}
void MouseState::_SetPosition(const Vector2i& pos) {
	position = pos;
}
void MouseState::_SetRelativePosition(const Vector2f& pos) {
	relativePosition = pos;
}
void MouseState::_SetButtonState(MouseButton button, ButtonState state) {
	buttonStates[(int)button] = state;
}
