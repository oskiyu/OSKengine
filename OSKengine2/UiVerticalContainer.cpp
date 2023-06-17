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

	float xDiff = 0.0f;
	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::LEFT))
		xDiff = 0;
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::CENTER_X))
		xDiff = 0;
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::RIGHT))
		xDiff = 0;

	child->_SetRelativePosition(Vector2f(
		child->GetMarging().x + GetPadding().x,
		relativeNextPosition + child->GetMarging().y
	));

	relativeNextPosition += child->GetSize().y + child->GetMarging().y + child->GetMarging().W;
}

void VerticalContainer::ResetLayout() {
	relativeNextPosition = 0.0f;
}
