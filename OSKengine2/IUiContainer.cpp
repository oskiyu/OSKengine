#include "IUiContainer.h"

#include "Transform2D.h"
#include "SpriteRenderer.h"

using namespace OSK::UI;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

void IContainer::AdjustSizeToChildren() {
	Vector2f newSize = 0.0f;

	for (const auto& child : children) {
		const Vector2f furthestPoint =
			child->GetSize() + child->GetRelativePosition() + Vector2f(child->GetMarging().Z, child->GetMarging().W);

		newSize = glm::max(newSize.ToGLM(), furthestPoint.ToGLM());
	}

	size = newSize + GetPadding2D();
}

void IContainer::AddChild(const std::string& key, SharedPtr<IElement> child) {
	children.Insert(child);
	childrenTable.Insert(key, child.GetPointer());

	EmplaceChild(child.GetPointer());
}

void IContainer::Rebuild() {
	ResetLayout();

	for (auto& child : children)
		EmplaceChild(child.GetPointer());
}

void IContainer::Render(SpriteRenderer* renderer, Vector2f parentPosition) const {
	if (sprite.GetView()) {
		Transform2D transform(EMPTY_GAME_OBJECT);
		transform.SetPosition(GetRelativePosition() + parentPosition);
		transform.SetScale(GetSize());

		renderer->Draw(sprite, transform);
	}

	for (const auto& child : children)
		child->Render(renderer, GetRelativePosition() + parentPosition);
}

void IContainer::UpdateByCursor(Vector2f cursorPosition, bool isPressed, Vector2f parentPosition) {
	for (const auto& child : children)
		child->UpdateByCursor(cursorPosition, isPressed, GetRelativePosition() + parentPosition);
}

IElement* IContainer::GetChild(const std::string_view name) const {
	return childrenTable.Get(static_cast<std::string>(name));
}