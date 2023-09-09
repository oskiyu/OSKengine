#include "UiElement.h"

#include <glm/ext/matrix_transform.hpp>

using namespace OSK;
using namespace OSK::UI;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

IElement::IElement(const Vector2f& size) :size(size) {

}


void IElement::Render(SpriteRenderer* renderer, Vector2f parentPosition) const {
	
}

void IElement::UpdateByCursor(Vector2f cursorPosition, bool isPressed, Vector2f parentPosition) {

}

void IElement::SetKeepRelativeSize(bool keepRelativeSize) {
	this->keepRelativeSize = keepRelativeSize;
}

bool IElement::KeepsRelativeSize() const {
	return keepRelativeSize;
}

void IElement::_SetRelativePosition(const Vector2f& relativePosition) {
	this->relativePosition = relativePosition;
}

void IElement::SetVisibility(bool visibility) {
	isVisible = visibility;
}

void IElement::SetVisible() {
	isVisible = true;
}

void IElement::SetInvisible() {
	isVisible = false;
}

bool IElement::IsVisible() const {
	return isVisible;
}

void IElement::Lock() {
	isLocked = true;
}

void IElement::Unlock() {
	isLocked = false;
}

bool IElement::IsLocked() const {
	return isLocked;
}

void IElement::SetPadding(const Vector2f& padding) {
	SetPadding(Vector4f(padding.x, padding.y, padding.x, padding.y));
}

void IElement::SetPadding(const Vector4f& padding) {
	this->padding = padding;
}

void IElement::SetMargin(const Vector2f& margin) {
	SetMargin(Vector4f(margin.x, margin.y, margin.x, margin.y));
}

void IElement::SetMargin(const Vector4f& margin) {
	this->margin = margin;
}

Vector2f IElement::GetRelativePosition() const {
	return relativePosition;
}

void IElement::SetSize(Vector2f newSize) {
	size = newSize;
}

Vector2f IElement::GetSize() const {
	return size;
}

Vector2f IElement::GetContentSize() const {
	return size - GetPadding2D();
}

Vector2f IElement::GetContentTopLeftPosition() const {
	return relativePosition + Vector2f(padding.x, padding.y);
}

Vector4f IElement::GetRectangle(Vector2f parentPosition) const {
	return Vector4f(
		relativePosition.x + parentPosition.x,
		relativePosition.y + parentPosition.y,
		size.x,
		size.y
	);
}

void IElement::SetAnchor(Anchor anchor) {
	this->anchor = anchor;
}

Vector4f IElement::GetPadding() const {
	return padding;
}

Vector4f IElement::GetMarging() const {
	return margin;
}

Vector2f IElement::GetPadding2D() const {
	return Vector2f(
		padding.x + padding.Z,
		padding.y + padding.W
	);
}

Vector2f IElement::GetMarging2D() const {
	return Vector2f(
		margin.x + margin.Z,
		margin.y + margin.W
	);
}
