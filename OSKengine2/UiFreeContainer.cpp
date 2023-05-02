#include "UiFreeContainer.h"

using namespace OSK;
using namespace OSK::UI;

FreeContainer::FreeContainer(const Vector2f& size) : IContainer(size) {

}

void FreeContainer::EmplaceChild(IElement* child) {
	Vector2f parentOffset = 0.0f;
	Vector2f childOffset = 0.0f;

	// Eje X
	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::LEFT)) {
		parentOffset.X = 0.0f;
		childOffset.X = 0.0f;
	}
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::CENTER_X)) {
		parentOffset.X = GetSize().X * 0.5f;
		childOffset.X = -child->GetSize().X * 0.5f;
	}
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::RIGHT)) {
		parentOffset.X = GetSize().X;
		childOffset.X = -child->GetSize().X;
	}

	// Eje Y
	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::TOP)) {
		parentOffset.Y = 0.0f;
		childOffset.Y = 0.0f;
	}
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::CENTER_Y)) {
		parentOffset.Y = GetSize().Y * 0.5f;
		childOffset.Y = -child->GetSize().Y * 0.5f;
	} 
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::BOTTOM)) {
		parentOffset.Y = GetSize().Y;
		childOffset.Y = -child->GetSize().Y;
	}

	const Vector2f totalOffset = parentOffset + childOffset;

	child->_SetRelativePosition(totalOffset);
}

void FreeContainer::ResetLayout() {

}
