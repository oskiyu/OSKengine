#include "UiVerticalContainer.h"

using namespace OSK;
using namespace OSK::UI;
using namespace OSK;

VerticalContainer::VerticalContainer(const Vector2f& size) : IContainer(size) {
	relativeNextPosition = GetPadding().y;
}

void VerticalContainer::EmplaceChild(IElement* child) {
	if (relativeNextPosition < GetPadding().y)
		relativeNextPosition = GetPadding().y;

	// Offset vertical desde la izquierda.
	float horizontalPositionOffset = 0.0f;

	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::LEFT)) {
		horizontalPositionOffset = child->GetMarging().x + GetPadding().x;
	}
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::CENTER_X)) {
		horizontalPositionOffset = GetSize().x * 0.5f - (child->GetSize().x + child->GetMarging().x) * 0.5f;
	}
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::RIGHT)) {
		horizontalPositionOffset = GetContentSize().x - child->GetSize().x - child->GetMarging().Z;
	}

	child->_SetRelativePosition(Vector2f(
		horizontalPositionOffset,
		relativeNextPosition + child->GetMarging().y
	).ToVector2i().ToVector2f());

	relativeNextPosition += child->GetSize().y + child->GetMarging().y + child->GetMarging().W;
}

void VerticalContainer::ResetLayout() {
	relativeNextPosition = 0.0f;
}
