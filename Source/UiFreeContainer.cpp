#include "UiFreeContainer.h"

using namespace OSK;
using namespace OSK::UI;

FreeContainer::FreeContainer(const Vector2f& size) : IContainer(size) {

}

void FreeContainer::EmplaceChild(IElement* child) {
	Vector2f childOffset = Vector2f::Zero;

	// Eje X
	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::LEFT)) {
		childOffset.x = 0.0f + child->GetMarging().x;
	}
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::CENTER_X)) {
		childOffset.x = GetSize().x * 0.5f - child->GetSize().x * 0.5f;
	}
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::RIGHT)) {
		childOffset.x = GetSize().x - child->GetSize().x - child->GetMarging().Z;
	}

	// Eje Y
	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::TOP)) {
		childOffset.y = 0.0f + child->GetMarging().y;
	}
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::CENTER_Y)) {
		childOffset.y = GetSize().y * 0.5f - child->GetSize().y * 0.5f;
	} 
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::BOTTOM)) {
		childOffset.y = GetSize().y - child->GetSize().y - child->GetMarging().Z;
	}

	const Vector2f totalOffset = childOffset;

	child->_SetPosition(GetPosition() + totalOffset);
}

void FreeContainer::ResetLayout() {

}
