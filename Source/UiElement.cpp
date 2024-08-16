#include "UiElement.h"

#include "SdfBindlessRenderer2D.h"
#include <glm/ext/matrix_transform.hpp>

using namespace OSK;
using namespace OSK::UI;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

IElement::IElement(const Vector2f& size) : m_size(size) {

}


void IElement::Render(SdfBindlessRenderer2D* renderer) const {
	if (!IsVisible()) {
		return;
	}

	for (const auto& drawCall : m_drawCalls) {
		renderer->Draw(drawCall);
	}
}

bool IElement::UpdateByCursor(Vector2f cursorPosition, bool isPressed) {
	return GetRectangle().ContainsPoint(cursorPosition);
}

void IElement::UpdateByKeyboard(const IO::KeyboardState&, const IO::KeyboardState&) {

}

void IElement::SetKeepRelativeSize(bool keepRelativeSize) {
	m_keepRelativeSize = keepRelativeSize;
}

bool IElement::KeepsRelativeSize() const {
	return m_keepRelativeSize;
}

void IElement::_SetPosition(const Vector2f& newPosition) {
	const Vector2f previousPosition = m_position;
	m_position = newPosition;
	OnPositionChanged(previousPosition);
}

void IElement::AddDrawCall(const SdfDrawCall2D& drawCall) {
	m_drawCalls.Insert(drawCall);
}

void IElement::AddDrawCall(SdfDrawCall2D&& drawCall) {
	m_drawCalls.Insert(std::move(drawCall));
}

void IElement::AddAllDrawCalls(const DynamicArray<SdfDrawCall2D>& drawCalls) {
	m_drawCalls.InsertAll(drawCalls);
}

void IElement::AddAllDrawCalls(DynamicArray<SdfDrawCall2D>&& drawCalls) {
	m_drawCalls.InsertAll(drawCalls);
}

DynamicArray<GRAPHICS::SdfDrawCall2D>& IElement::GetAllDrawCalls() {
	return m_drawCalls;
}

const DynamicArray<GRAPHICS::SdfDrawCall2D>& IElement::GetAllDrawCalls() const {
	return m_drawCalls;
}

void IElement::SetVisibility(bool visibility) {
	m_isVisible = visibility;
}

void IElement::SetVisible() {
	m_isVisible = true;
}

void IElement::SetInvisible() {
	m_isVisible = false;
}

bool IElement::IsVisible() const {
	return m_isVisible;
}

void IElement::Lock() {
	m_isLocked = true;
}

void IElement::Unlock() {
	m_isLocked = false;
}

bool IElement::IsLocked() const {
	return m_isLocked;
}

void IElement::SetPadding(const Vector2f& padding) {
	SetPadding(Vector4f(padding.x, padding.y, padding.x, padding.y));
}

void IElement::SetPadding(const Vector4f& padding) {
	m_padding = padding;
}

void IElement::SetMargin(const Vector2f& margin) {
	SetMargin(Vector4f(margin.x, margin.y, margin.x, margin.y));
}

void IElement::SetMargin(const Vector4f& margin) {
	m_margin = margin;
}

Vector2f IElement::GetPosition() const {
	return m_position;
}

void IElement::SetSize(Vector2f newSize) {
	const Vector2f previousSize = m_size;
	m_size = newSize;
	OnSizeChanged(previousSize);
}

Vector2f IElement::GetSize() const {
	return m_size;
}

Vector2f IElement::GetContentSize() const {
	return m_size - GetPadding2D();
}

Vector2f IElement::GetContentTopLeftPosition() const {
	return m_position + Vector2f(m_padding.x, m_padding.y);
}

Vector4f IElement::GetRectangle() const {
	return Vector4f(
		m_position.x,
		m_position.y,
		m_size.x,
		m_size.y
	);
}

void IElement::SetAnchor(Anchor anchor) {
	m_anchor = anchor;
}

Vector4f IElement::GetPadding() const {
	return m_padding;
}

Vector4f IElement::GetMarging() const {
	return m_margin;
}

Vector2f IElement::GetPadding2D() const {
	return Vector2f(
		m_padding.x + m_padding.z,
		m_padding.y + m_padding.w
	);
}

Vector2f IElement::GetMarging2D() const {
	return Vector2f(
		m_margin.x + m_margin.z,
		m_margin.y + m_margin.w
	);
}

void IElement::_SetParent(IContainer* parent) {
	m_parent = parent;
}

void IElement::RepositionDrawCalls(const Vector2f& positionDiff) {
	for (SdfDrawCall2D& drawCall : m_drawCalls) {
		drawCall.transform.AddPosition(positionDiff);
	}
}

void IElement::ResizeDrawCalls(const Vector2f& sizeDiff) {
	for (SdfDrawCall2D& drawCall : m_drawCalls) {
		drawCall.transform.SetScale(drawCall.transform.GetScale() * sizeDiff);
	}
}

void IElement::OnPositionChanged(const Vector2f& previousPosition) {
	const Vector2f positionDiff = m_position - previousPosition;
	RepositionDrawCalls(positionDiff);
}

void IElement::OnSizeChanged(const Vector2f& previousSize) {
	// if (m_keepRelativeSize) {
		const Vector2f sizeDiff = m_size / previousSize;
		ResizeDrawCalls(sizeDiff);
	// }
}

IContainer* IElement::GetParent() {
	return m_parent;
}

bool IElement::HasParent() const {
	return m_parent;
}
