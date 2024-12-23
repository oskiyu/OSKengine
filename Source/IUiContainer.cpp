#include "IUiContainer.h"

#include "TransformComponent2D.h"
#include "SpriteRenderer.h"
#include "InvalidArgumentException.h"

using namespace OSK::UI;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

void IContainer::SetSize(Vector2f newSize) {
	const Vector2f oldSize = GetSize();

	IElement::SetSize(newSize);

	for (auto& child : m_children) {
		if (child->KeepsRelativeSize()) {
			const Vector2f ratio = child->GetSize() / oldSize;
			child->SetSize(newSize * ratio);
		}
	}

	Rebuild();
}

void IContainer::_SetPosition(const Vector2f& newPosition) {
	IElement::_SetPosition(newPosition);
	Rebuild();
}

void IContainer::AdjustSizeToChildren() {
	Vector2f newSize = Vector2f::Zero;

	for (const auto& child : m_children) {
		if (!child->IsVisible()) {
			continue;
		}

		const Vector2f childRelativePosition = child->GetPosition() - GetPosition();
		const Vector2f childRightBottomMargings = Vector2f(child->GetMarging().z, child->GetMarging().w);

		const Vector2f furthestPoint = child->GetSize() + childRelativePosition + childRightBottomMargings;

		newSize = Vector2f(glm::max(newSize.ToGlm(), furthestPoint.ToGlm()));
	}

	SetSize(newSize + GetPadding2D());
}

void IContainer::AddChild(const std::string& key, OwnedPtr<IElement> child) {
	m_children.Insert(child.GetPointer());
	m_childrenTable[key] = child.GetPointer();

	child->_SetParent(this);

	EmplaceChild(child.GetPointer());
}

void IContainer::Rebuild() {
	ResetLayout();

	for (auto& child : m_children)
		EmplaceChild(child.GetPointer());

	for (auto& child : m_children)
		if (child->Is<IContainer>())
			child->As<IContainer>()->Rebuild();
}

void IContainer::Render(ISdfRenderer2D* renderer) const {
	IElement::Render(renderer);

	for (const auto& child : m_children) {
		if (child->IsVisible()) {
			child->Render(renderer);
		}
	}
}

bool IContainer::UpdateByCursor(Vector2f cursorPosition, bool isPressed) {
	if (IsLocked() || !IsVisible()) {
		return false;
	}

	for (auto& child : m_children) {
		bool processed = child->UpdateByCursor(cursorPosition, isPressed);

		if (processed)
			return true;
	}

	return false;
}

void IContainer::UpdateByKeyboard(const IO::KeyboardState& previous, const IO::KeyboardState& current) {
	if (IsLocked() || !IsVisible()) {
		return;
	}

	for (auto& child : m_children) {
		if (!(child->IsLocked() || !child->IsVisible())) {
			child->UpdateByKeyboard(previous, current);
		}
	}
}

IElement* IContainer::GetChild(const std::string_view name) {
	return m_childrenTable.find(name)->second;
}

const IElement* IContainer::GetChild(const std::string_view name) const {
	return m_childrenTable.find(name)->second;
}

void IContainer::DeleteChild(std::string_view childName) {
	auto iterator = m_childrenTable.find(childName);
	OSK_ASSERT(iterator != m_childrenTable.end(), InvalidArgumentException(std::format("No existe el hijo {}", childName)));

	for (USize64 i = 0; i < m_children.GetSize(); i++) {
		if (m_children[i].GetPointer() == iterator->second) {
			m_children.RemoveAt(i);
			break;
		}
	}

	m_childrenTable.erase(iterator);

	Rebuild();
}

bool IContainer::HasChild(std::string_view childName) const {
	return m_childrenTable.contains(childName);
}
