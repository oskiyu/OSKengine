#include "UiDropdown.h"

#include "Font.h"
#include "FontInstance.h"

#include "UnreachableException.h"

using namespace OSK;

UI::Dropdown::Element::Element(const Vector2f& size, const std::string& text) : IElement(size), m_textView(size), m_defaultImageView(size), m_hoveredImageView(size) {
	m_textView.SetText(text);
	m_textView.AdjustSizeToText();
}

void UI::Dropdown::Element::Render(GRAPHICS::ISdfRenderer2D* renderer) const {
	if (!IsVisible())
		return;

	m_state == State::DEFAULT
		? m_defaultImageView.Render(renderer)
		: m_hoveredImageView.Render(renderer);

	m_textView.Render(renderer);
}

void UI::Dropdown::Element::_SetPosition(const Vector2f& newPosition) {
	IElement::_SetPosition(newPosition);

	m_textView._SetPosition(newPosition + GetTextOffset());
}

void UI::Dropdown::Element::SetBackground(const GRAPHICS::IGpuImageView* view, const Color& defaultColor, const Color& hoveredColor) {
	GRAPHICS::SdfDrawCall2D drawCall{};
	drawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::TEXTURE;
	drawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
	drawCall.texture = view;

	drawCall.mainColor = defaultColor;
	drawCall.transform.SetPosition(m_defaultImageView.GetContentTopLeftPosition());
	drawCall.transform.SetScale(m_defaultImageView.GetSize());

	m_defaultImageView.AddDrawCall(drawCall);

	drawCall.mainColor = hoveredColor;
	drawCall.transform.SetPosition(m_hoveredImageView.GetContentTopLeftPosition());
	drawCall.transform.SetScale(m_hoveredImageView.GetSize());

	m_hoveredImageView.AddDrawCall(drawCall);
}

void UI::Dropdown::Element::SetFont(ASSETS::AssetRef<ASSETS::Font> fuente, USize32 fontSize) {
	m_textView.SetFont(fuente);
	m_textView.SetFontSize(fontSize);
	m_textView.AdjustSizeToText();

	m_textView._SetPosition(GetPosition() + GetTextOffset());
}

Vector2f UI::Dropdown::Element::GetTextOffset() const {
	if (!m_textView.GetFont())
		return Vector2f::Zero;
	
	return Vector2f(
		m_textView.GetMarging().y,
		GetSize().y * 0.5f - m_textView.GetSize().y * 0.5f
	);
}

std::string_view UI::Dropdown::Element::GetText() const {
	return m_textView.GetText();
}

void UI::Dropdown::Element::SetState(State state) {
	m_state = state;
}


// Dropdown

UI::Dropdown::Dropdown(const Vector2f& size) : IElement(size) {

}

void UI::Dropdown::Render(GRAPHICS::ISdfRenderer2D* renderer) const {
	if (!IsVisible() || m_elements.IsEmpty())
		return;

	Vector2f offset = GetContentTopLeftPosition();

	const Element& firstElement = m_elements[m_selectedElement];
	firstElement.Render(renderer);
	offset.y += firstElement.GetSize().y;

	if (m_state == State::EXPANDED) {
		for (UIndex64 i = 0; i < m_elements.GetSize(); i++) {
			if (i == m_selectedElement)
				continue;

			const Element& e = m_elements[i];

			e.Render(renderer);
			offset.y += e.GetSize().y;
		}
	}
}

bool UI::Dropdown::UpdateByCursor(Vector2f cursor, bool isPressed) {
	if (!IsVisible() || IsLocked() || m_elements.IsEmpty())
		return false;

	if (m_state == State::DEFAULT) {
		const bool isMouseOver = GetRectangle().ContainsPoint(cursor);

		m_elements[m_selectedElement].SetState(isMouseOver ? Element::State::HOVERED : Element::State::DEFAULT);

		if (isMouseOver && isPressed && !m_wasPreviousFramePressed) {
			m_state = State::EXPANDED;
			m_wasPreviousFramePressed = true;
		} else
		if (!isMouseOver || !isPressed) {
			m_wasPreviousFramePressed = false;
		}

		return isMouseOver;
	}

	else if (m_state == State::EXPANDED) {
		Vector2f offset = GetContentTopLeftPosition();

		Element& firstElement = m_elements[m_selectedElement];
		bool isMouseOver = firstElement.GetRectangle().ContainsPoint(cursor);

		firstElement.SetState(isMouseOver ? Element::State::HOVERED : Element::State::DEFAULT);

		if (isMouseOver && isPressed && !m_wasPreviousFramePressed) {
			m_state = State::DEFAULT;
			m_wasPreviousFramePressed = true;

			return true;
		}

		m_wasPreviousFramePressed = isMouseOver && isPressed;

		offset.y += firstElement.GetSize().y;

		for (UIndex64 i = 0; i < m_elements.GetSize(); i++) {
			if (i == m_selectedElement)
				continue;

			Element& e = m_elements[i];

			isMouseOver = e.GetRectangle().ContainsPoint(cursor);

			e.SetState(isMouseOver ? Element::State::HOVERED : Element::State::DEFAULT);

			if (isMouseOver && isPressed) {
				SelectOption(e.GetText());
				m_wasPreviousFramePressed = true;

				return true;
			}

			offset.y += e.GetSize().y;

			if (isMouseOver)
				return true;
		}

		return false;
	}

	OSK_ASSERT(false, UnreachableException("Valor de DropdownState sin contemblar."));
	return false;
}

void UI::Dropdown::AddElement(const std::string& text) {
	RemoveElement(text);

	Element element(GetSize(), text);
	element.SetFont(m_fuente, m_fontSize);
	element.SetBackground(m_elementsBackground, m_defaultColor, m_hoverColor);

	m_elements.Insert(std::move(element));

	m_elementsMap[text] = m_elements.GetSize() - 1;
}

bool UI::Dropdown::ContainsElement(std::string_view text) const {
	return m_elementsMap.contains(text);
}

void UI::Dropdown::RemoveElement(std::string_view text) {
	if (!m_elementsMap.contains(text))
		return;

	const UIndex64 elementIndex = m_elementsMap.find(text)->second;
	m_elements.RemoveAt(elementIndex);
	m_elementsMap.erase(text);

	// Actualización del mapa.
	for (UIndex64 n = elementIndex; n < m_elements.GetSize(); n++)
		m_elementsMap[static_cast<std::string>(m_elements[n].GetText())] = n;
}

void UI::Dropdown::SelectOption(std::string_view selectedText) {
	if (!ContainsElement(selectedText))
		return;

	m_selectedElement = m_elementsMap.find(selectedText)->second;
	m_callback(selectedText);

	m_state = State::DEFAULT;
}

void UI::Dropdown::SetFont(ASSETS::AssetRef<ASSETS::Font> fuente, USize32 fontSize) {
	m_fuente = fuente;
	m_fontSize = fontSize;

	for (Element& e : m_elements)
		e.SetFont(m_fuente, m_fontSize);
}

void UI::Dropdown::SetBackground(const GRAPHICS::IGpuImageView* view, const OSK::Color& defaultColor, const OSK::Color& hoverColor) {
	m_elementsBackground = view;

	m_defaultColor = defaultColor;
	m_hoverColor = hoverColor;

	for (Element& e : m_elements)
		e.SetBackground(m_elementsBackground, m_defaultColor, m_hoverColor);
}

void UI::Dropdown::SetSelectionCallback(const CallbackFnc& callback) {
	m_callback = callback;
}
