#include "UiElement.h"

#include <ext\matrix_transform.hpp>

using namespace OSK;
using namespace OSK::UI;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void UiElement::AddChildElement(OwnedPtr<UiElement> element) {
	childElements.InsertMove(std::move(UniquePtr<UiElement>(element.GetPointer())));
}

void UiElement::AddChildElement(OwnedPtr<UiElement> element, const std::string& key) {
	AddChildElement(element);
	childElementsTable.InsertMove(key, element.GetPointer());
}

void UiElement::SetPosition(const Vector2f& pos) {
	rectangle.X = pos.X;
	rectangle.Y = pos.Y;
}

void UiElement::SetSize(const Vector2f& size) {
	rectangle.Z = size.X;
	rectangle.W = size.Y;
}

Vector2f UiElement::GetPosition() const {
	return rectangle.GetRectanglePosition();
}

Vector2f UiElement::GetSize() const {
	return rectangle.GetRectangleSize();
}

Vector4f UiElement::GetRectangle() const {
	return rectangle;
}

UiElement* UiElement::GetChildElement(const std::string& name) {
	return childElementsTable.Get(name);
}

glm::mat4 UiElement::GetMatrix() const {
	return glm::scale(
		glm::translate(glm::mat4(1.0f), { GetPosition().X, GetPosition().Y, 0.0f }),
		{ GetSize().X, GetSize().Y, 0.0f });
}

const LinkedList<UniquePtr<UiElement>>& UiElement::GetChildElements() const {
	return childElements;
}
