#include "UiTextInput.h"

#include "KeyboardState.h"
#include "PrintableKeys.h"

#include "SpriteRenderer.h"
#include "Transform2D.h"
#include "SdfStringInfo.h"
#include "SdfBindlessRenderer2D.h"


using namespace OSK;
using namespace OSK::UI;
using namespace OSK::IO;


void TextInput::SetFontSize(USize32 size) {
	m_fontSize = size;
	m_font->LoadSizedFont(size);
}

void TextInput::SetFont(ASSETS::AssetRef<ASSETS::Font> font) {
	m_font = font;
}


void TextInput::SetText(const std::string& text) {
	m_text = text;
}

void TextInput::SetPlaceholderText(const std::string& text) {
	m_text = text;
	m_hasPlaceholderText = true;
}


std::string_view TextInput::GetText() const {
	return m_text;
}

bool TextInput::HasPlaceholderText() const {
	return m_hasPlaceholderText;
}


void TextInput::UpdateByKeyboard(const IO::KeyboardState& previous, const IO::KeyboardState& current) {
	const bool isMayus = current.IsKeyDown(Key::LEFT_SHIFT);

	for (const auto key : PrintableKeys) {
		if (previous.IsKeyUp(key) && current.IsKeyDown(key)) {
			if (m_hasPlaceholderText) {
				m_text = "";
				m_hasPlaceholderText = false;
			}

			m_text.push_back(GetPrintableKeyText(key, isMayus));
		}
	}

	if (current.IsKeyDown(Key::BACKSPACE) && previous.IsKeyUp(Key::BACKSPACE) && !m_text.empty()) {
		m_text.pop_back();
	}
}


void TextInput::Render(GRAPHICS::ISdfRenderer2D* renderer) const {
	if (!m_font.GetAsset()) {
		return;
	}

	Vector2f globalPosition = GetContentTopLeftPosition();
	globalPosition.y += GetContentSize().y;
	globalPosition = globalPosition.ToVector2i().ToVector2f();

	GRAPHICS::SdfStringInfo info{};
	info.text = m_text;
	info.font = &m_font->GetExistingInstance(m_fontSize);
	info.transform = ECS::Transform2D(ECS::EMPTY_GAME_OBJECT);
	info.transform.SetPosition(globalPosition);

	renderer->Draw(info);
}
