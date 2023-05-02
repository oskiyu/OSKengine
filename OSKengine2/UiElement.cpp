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

void IElement::_SetRelativePosition(const Vector2f& relativePosition) {
	this->relativePosition = relativePosition;
}

void IElement::SetPadding(const Vector2f& padding) {
	SetPadding(Vector4f(padding.X, padding.Y, padding.X, padding.Y));
}

void IElement::SetPadding(const Vector4f& padding) {
	this->padding = padding;
}

void IElement::SetMargin(const Vector2f& margin) {
	SetMargin(Vector4f(margin.X, margin.Y, margin.X, margin.Y));
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
	return relativePosition + Vector2f(padding.X, padding.Y);
}

Vector4f IElement::GetRectangle(Vector2f parentPosition) const {
	return Vector4f(
		relativePosition.X + parentPosition.X,
		relativePosition.Y + parentPosition.Y,
		size.X,
		size.Y
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
		padding.X + padding.Z,
		padding.Y + padding.W
	);
}

Vector2f IElement::GetMarging2D() const {
	return Vector2f(
		margin.X + margin.Z,
		margin.Y + margin.W
	);
}
