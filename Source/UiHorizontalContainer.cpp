#include "UiHorizontalContainer.h"

using namespace OSK;
using namespace OSK::UI;
using namespace OSK;

HorizontalContainer::HorizontalContainer(const Vector2f& size) : IContainer(size) {
	
}

void HorizontalContainer::EmplaceChild(IElement* child) {
	if (m_relativeNextPosition < GetPadding().x) {
		m_relativeNextPosition = GetPadding().x;
	}

	// Offset vertical desde arriba.
	float verticalPositionOffset = 0.0f;

	if (EFTraits::HasFlag(child->GetAnchor(), Anchor::TOP)) 
	{
		verticalPositionOffset = child->GetMarging().y + GetPadding().y;
	} 
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::CENTER_Y)) 
	{
		verticalPositionOffset = GetSize().y * 0.5f - (child->GetSize().y + child->GetMarging().y) * 0.5f;
	} 
	else if (EFTraits::HasFlag(child->GetAnchor(), Anchor::BOTTOM)) 
	{
		verticalPositionOffset = GetContentSize().y - child->GetSize().y - child->GetMarging().W;
	}

	const Vector2f childPosition = Vector2f(
		GetContentTopLeftPosition().x + m_relativeNextPosition + child->GetMarging().x,
		GetContentTopLeftPosition().y + verticalPositionOffset);

	child->_SetPosition(childPosition.ToVector2i().ToVector2f());

	m_relativeNextPosition += child->GetSize().x + child->GetMarging().x + child->GetMarging().Z;
}

void HorizontalContainer::ResetLayout() {
	m_relativeNextPosition = 0.0f;
}
