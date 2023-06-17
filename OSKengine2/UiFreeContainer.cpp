#include "UiFreeContainer.h"

using namespace OSK;
using namespace OSK::UI;

FreeContainer::FreeContainer(const Vector2f& size) : IContainer(size) {

}

void FreeContainer::EmplaceChild(IElement* child) {
	Vector2f parentOffset = Vector2f::Zero;
	Vector2f childOffset = Vector2f::Zero;

	// Eje X
	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::LEFT)) {
		parentOffset.x = 0.0f;
		childOffset.x = 0.0f;
	}
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::CENTER_X)) {
		parentOffset.x = GetSize().x * 0.5f;
		childOffset.x = -child->GetSize().x * 0.5f;
	}
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::RIGHT)) {
		parentOffset.x = GetSize().x;
		childOffset.x = -child->GetSize().x;
	}

	// Eje Y
	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::TOP)) {
		parentOffset.y = 0.0f;
		childOffset.y = 0.0f;
	}
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::CENTER_Y)) {
		parentOffset.y = GetSize().y * 0.5f;
		childOffset.y = -child->GetSize().y * 0.5f;
	} 
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::BOTTOM)) {
		parentOffset.y = GetSize().y;
		childOffset.y = -child->GetSize().y;
	}

	const Vector2f totalOffset = parentOffset + childOffset;

	child->_SetRelativePosition(totalOffset);
}

void FreeContainer::ResetLayout() {

}
