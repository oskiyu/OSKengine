#include "UiBorderLayout.h"

#include "InvalidObjectStateException.h"

using namespace OSK;
using namespace OSK::UI;


BorderLayout::BorderLayout(const Vector2f& size) : IContainer(size) {

}

void BorderLayout::AddChild_InPosition(SharedPtr<IElement> child, Position position) {
	switch (position) {
	case OSK::UI::BorderLayout::Position::NORTH:
		m_northElement = child.GetPointer();
		AddChild("0north", child);
		break;
	case OSK::UI::BorderLayout::Position::SOUTH:
		m_southElement = child.GetPointer();
		AddChild("1south", child);
		break;
	case OSK::UI::BorderLayout::Position::EAST:
		m_eastElement = child.GetPointer();
		AddChild("2east", child);
		break;
	case OSK::UI::BorderLayout::Position::WEST:
		m_westElement = child.GetPointer();
		AddChild("3west", child);
		break;
	case OSK::UI::BorderLayout::Position::CENTER:
		m_centerElement = child.GetPointer();
		AddChild("4center", child);
		break;
	default:
		break;
	}
}

void BorderLayout::EmplaceChild(IElement* child) {
	Position childPosition = Position::CENTER;
	if (child == m_northElement) {
		childPosition = Position::NORTH;
	}
	else if (child == m_southElement) {
		childPosition = Position::SOUTH;
	}
	else if (child == m_eastElement) {
		childPosition = Position::EAST;
	}
	else if (child == m_westElement) {
		childPosition = Position::WEST;
	}

	const float topStartingPosition = m_northElement
		? m_northElement->GetSize().y + m_northElement->GetMarging().W
		: 0.0f;

	const float bottomStartingPosition = m_southElement
		? GetSize().y - (m_southElement->GetSize().y + m_southElement->GetMarging().y)
		: GetSize().y;

	const float leftStartingPosition = m_eastElement
		? m_eastElement->GetSize().x + m_eastElement->GetMarging().x
		: 0.0f;

	const float rightStartingPosition = m_westElement
		? GetSize().x - (m_westElement->GetSize().x + m_westElement->GetMarging().Z)
		: GetSize().x;

	switch (childPosition) {
	case OSK::UI::BorderLayout::Position::NORTH:
	{
		auto size = child->GetSize();
		size.x = GetSize().x;
		child->SetSize(size.ToVector2i().ToVector2f());

		child->_SetRelativePosition(Vector2f::Zero);
	}
		break;

	case OSK::UI::BorderLayout::Position::SOUTH:
	{
		auto size = child->GetSize();
		size.x = GetSize().x;
		child->SetSize(size.ToVector2i().ToVector2f());

		Vector2f relativePosition = Vector2f::Zero;
		relativePosition.y = GetSize().y - size.y;
		child->_SetRelativePosition(relativePosition.ToVector2i().ToVector2f());
	}
		break;

	case OSK::UI::BorderLayout::Position::EAST:
	{
		auto size = child->GetSize();
		size.y = bottomStartingPosition - topStartingPosition;
		child->SetSize(size.ToVector2i().ToVector2f());

		Vector2f relativePosition = Vector2f::Zero;
		relativePosition.y = topStartingPosition;
		child->_SetRelativePosition(relativePosition.ToVector2i().ToVector2f());
	}
		break;

	case OSK::UI::BorderLayout::Position::WEST:
	{
		auto size = child->GetSize();
		size.y = bottomStartingPosition - topStartingPosition;
		child->SetSize(size.ToVector2i().ToVector2f());

		Vector2f relativePosition = Vector2f::Zero;
		relativePosition.x = GetSize().x - size.x;
		relativePosition.y = topStartingPosition;
		child->_SetRelativePosition(relativePosition.ToVector2i().ToVector2f());
	}
		break;

	case OSK::UI::BorderLayout::Position::CENTER:
	{
		auto size = child->GetSize();
		size.x = rightStartingPosition - leftStartingPosition;
		size.y = bottomStartingPosition - topStartingPosition;
		child->SetSize(size.ToVector2i().ToVector2f());

		Vector2f relativePosition = Vector2f::Zero;
		relativePosition.x = leftStartingPosition;
		relativePosition.y = topStartingPosition;
		child->_SetRelativePosition(relativePosition.ToVector2i().ToVector2f());
	}
		break;
	}
}

void BorderLayout::ResetLayout() {

}

bool BorderLayout::HasChild(Position position) const {
	switch (position) {
	case OSK::UI::BorderLayout::Position::NORTH:
		return m_northElement;
		break;
	case OSK::UI::BorderLayout::Position::SOUTH:
		return m_southElement;
		break;
	case OSK::UI::BorderLayout::Position::EAST:
		return m_eastElement;
		break;
	case OSK::UI::BorderLayout::Position::WEST:
		return m_westElement;
		break;
	case OSK::UI::BorderLayout::Position::CENTER:
		return m_centerElement;
		break;
	}
}

IElement* BorderLayout::GetChild(Position position) {
	OSK_ASSERT(
		HasChild(position), 
		InvalidObjectStateException(std::format("No existe un elemento en esa posición.")));

	switch (position) {
	case OSK::UI::BorderLayout::Position::NORTH:
		return m_northElement;
		break;
	case OSK::UI::BorderLayout::Position::SOUTH:
		return m_southElement;
		break;
	case OSK::UI::BorderLayout::Position::EAST:
		return m_eastElement;
		break;
	case OSK::UI::BorderLayout::Position::WEST:
		return m_westElement;
		break;
	case OSK::UI::BorderLayout::Position::CENTER:
		return m_centerElement;
		break;
	}
}

const IElement* BorderLayout::GetChild(Position position) const {
	OSK_ASSERT(
		HasChild(position),
		InvalidObjectStateException(std::format("No existe un elemento en esa posición.")));

	switch (position) {
	case OSK::UI::BorderLayout::Position::NORTH:
		return m_northElement;
		break;
	case OSK::UI::BorderLayout::Position::SOUTH:
		return m_southElement;
		break;
	case OSK::UI::BorderLayout::Position::EAST:
		return m_eastElement;
		break;
	case OSK::UI::BorderLayout::Position::WEST:
		return m_westElement;
		break;
	case OSK::UI::BorderLayout::Position::CENTER:
		return m_centerElement;
		break;
	}
}
