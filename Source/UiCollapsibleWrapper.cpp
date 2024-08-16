#include "UiCollapsibleWrapper.h"

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::UI;

CollapsibleWrapper::CollapsibleWrapper(OwnedPtr<IElement> content, const std::string& title, ASSETS::AssetRef<ASSETS::Font> font, USize32 fontSize) : VerticalContainer(content->GetSize()), m_content(content.GetPointer()) {
	m_bar = new Button(content->GetSize());
	
	m_bar->SetTextFont(font);
	m_bar->SetTextFontSize(fontSize);
	m_bar->SetText(title);

	/**/ 
	/**/

	const auto textSize = font->GetInstance(fontSize).GetTextSize(title);

	// m_bar->SetSize(textSize + m_bar->GetPadding2D());

	m_bar->SetCallback([this](bool isPressed) {
		isPressed
			? Collapse()
			: Expand();
		});

	AddChild("button", m_bar);
	AddChild("content", m_content);
}

IElement* CollapsibleWrapper::GetContent() {
	return m_content;
}

const IElement* CollapsibleWrapper::GetContent() const {
	return m_content;
}

Button* CollapsibleWrapper::GetToggleBar() {
	return m_bar;
}

const Button* CollapsibleWrapper::GetToggleBar() const {
	return m_bar;
}

void CollapsibleWrapper::AdjustToggleBarSize() {
	m_bar->SetSize(Vector2f(
		glm::max(m_bar->GetSize().x, m_content->GetSize().x),
		m_bar->GetSize().y));
}

void CollapsibleWrapper::Expand() {
	Vector2f totalSize = m_bar->GetSize();
	totalSize.y += m_content->GetSize().y + m_content->GetMarging2D().y;

	SetSize(totalSize);

	m_content->SetVisible();

	if (HasParent()) {
		GetParent()->Rebuild();
	}
}

void CollapsibleWrapper::Collapse() {
	SetSize(m_bar->GetSize());
	m_content->SetInvisible();

	if (HasParent()) {
		GetParent()->Rebuild();
	}
}
