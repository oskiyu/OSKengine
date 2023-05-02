#include "UiHorizontalContainer.h"

using namespace OSK;
using namespace OSK::UI;
using namespace OSK;

HorizontalContainer::HorizontalContainer(const Vector2f& size) : IContainer(size) {
	
}

void HorizontalContainer::EmplaceChild(IElement* child) {
	if (relativeNextPosition < GetPadding().X)
		relativeNextPosition = GetPadding().X;

	// Offset vertical desde arriba.
	float verticalPositionOffset = 0.0f;

	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::TOP)) {
		verticalPositionOffset = child->GetMarging().Y + GetPadding().Y;
	} 
	else
	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::CENTER_Y)) {
		verticalPositionOffset = GetSize().Y * 0.5f - (child->GetSize().Y + child->GetMarging().Y) * 0.5;
	} 
	else
	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::BOTTOM)) {
		verticalPositionOffset = GetContentSize().Y - child->GetSize().Y - child->GetMarging().W;
	}

	child->_SetRelativePosition(Vector2f(
		relativeNextPosition + child->GetMarging().X,
		verticalPositionOffset
	).ToVector2i().ToVector2f());

	relativeNextPosition += child->GetSize().X + child->GetMarging().X + child->GetMarging().Z;
}

void HorizontalContainer::ResetLayout() {
	relativeNextPosition = 0.0f;
}
