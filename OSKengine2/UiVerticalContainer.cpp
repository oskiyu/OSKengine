#include "UiVerticalContainer.h"

using namespace OSK;
using namespace OSK::UI;
using namespace OSK;

VerticalContainer::VerticalContainer(const Vector2f& size) : IContainer(size) {
	relativeNextPosition = GetPadding().Y;
}

void VerticalContainer::EmplaceChild(IElement* child) {
	if (relativeNextPosition < GetPadding().Y)
		relativeNextPosition = GetPadding().Y;

	float xDiff = 0.0f;
	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::LEFT))
		xDiff = 0;
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::CENTER_X))
		xDiff = 0;
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::RIGHT))
		xDiff = 0;

	child->_SetRelativePosition(Vector2f(
		child->GetMarging().X + GetPadding().X,
		relativeNextPosition + child->GetMarging().Y
	));

	relativeNextPosition += child->GetSize().Y + child->GetMarging().Y + child->GetMarging().W;
}

void VerticalContainer::ResetLayout() {
	relativeNextPosition = 0.0f;
}
