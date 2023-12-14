#include "IUiContainer.h"

#include "Transform2D.h"
#include "SpriteRenderer.h"

using namespace OSK::UI;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

void IContainer::SetSize(Vector2f newSize) {
	const Vector2f oldSize = GetSize();

	IElement::SetSize(newSize);

	for (auto& child : children) {
		if (child->KeepsRelativeSize()) {
			const Vector2f ratio = child->GetSize() / oldSize;
			child->SetSize(newSize * ratio);
		}
	}

	Rebuild();
}

void IContainer::AdjustSizeToChildren() {
	Vector2f newSize = Vector2f::Zero;

	for (const auto& child : children) {
		const Vector2f furthestPoint =
			child->GetSize() + child->GetRelativePosition() + Vector2f(child->GetMarging().Z, child->GetMarging().W);

		newSize = Vector2f(glm::max(newSize.ToGlm(), furthestPoint.ToGlm()));
	}

	size = newSize + GetPadding2D();
}

void IContainer::AddChild(const std::string& key, SharedPtr<IElement> child) {
	children.Insert(child);
	childrenTable[key] = child.GetPointer();

	EmplaceChild(child.GetPointer());
}

void IContainer::Rebuild() {
	ResetLayout();

	for (auto& child : children)
		EmplaceChild(child.GetPointer());
}

void IContainer::Render(SpriteRenderer* renderer, Vector2f parentPosition) const {
	if (!IsVisible()) 
		return;

	if (sprite.GetView()) {
		Transform2D transform(EMPTY_GAME_OBJECT);
		transform.SetPosition(GetRelativePosition() + parentPosition);
		transform.SetScale(GetSize());

		renderer->Draw(sprite, transform);
	}

	for (const auto& child : children)
		if (child->IsVisible())
			child->Render(renderer, GetRelativePosition() + parentPosition);
}

bool IContainer::UpdateByCursor(Vector2f cursorPosition, bool isPressed, Vector2f parentPosition) {
	if (IsLocked() || !IsVisible())
		return false;

	for (auto& child : children) {
		bool processed = child->UpdateByCursor(cursorPosition, isPressed, GetRelativePosition() + parentPosition);

		if (processed)
			return true;
	}

	return false;
}

IElement* IContainer::GetChild(const std::string_view name) {
	return childrenTable.at(static_cast<std::string>(name));
}

const IElement* IContainer::GetChild(const std::string_view name) const {
	return childrenTable.at(static_cast<std::string>(name));
}
