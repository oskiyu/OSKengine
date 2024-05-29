#include "UiConsole.h"

#include "KeyboardState.h"
#include "OSKengine.h"

using namespace OSK;
using namespace OSK::UI;

Console::Console(const Vector2f& size) : VerticalContainer(size) {
	this->SetPadding(Vector2f(20.0f));

	const Vector2f msgSize = {
		size.x, 20.0f
	};

	for (UIndex64 i = 0; i < MaxMessages; i++) {
		auto* textView = new TextView(msgSize);
		AddChild(std::to_string(i), textView);

		m_textMessages[MaxMessages - 1 - i] = textView;
		textView->SetInvisible();
	}

	auto* textInput = new TextInput(msgSize);
	AddChild("input", textInput);

	m_textInput = textInput;

	this->AdjustSizeToChildren();
}

void Console::SetFont(ASSETS::AssetRef<ASSETS::Font> font) {
	for (auto* msg : m_textMessages) {
		msg->SetFont(font);
	}
	m_textInput->SetFont(font);
}

void Console::SetFontSize(USize32 fontSize) {
	for (auto* msg : m_textMessages) {
		msg->SetFontSize(fontSize);
	}
	m_textInput->SetFontSize(fontSize);
}

void Console::AddMessage(const std::string& msg) {
	if (m_nextMessageIndex) {
		for (UIndex32 i = m_nextMessageIndex - 1; i > 0; i--) {
			m_textMessages[i]->SetText(static_cast<std::string>(m_textMessages[i - 1]->GetText()));
		}
	}

	m_textMessages[0]->SetText(msg);

	m_textMessages[m_nextMessageIndex]->SetVisible();
	
	if (m_nextMessageIndex < MaxMessages - 1) {
		m_nextMessageIndex++;
	}
}

void Console::UpdateByKeyboard(const IO::KeyboardState& previous, const IO::KeyboardState& current) {
	if (
		current.IsKeyDown(IO::Key::ENTER) &&
		previous.IsKeyUp(IO::Key::ENTER) &&
		!m_textInput->GetText().empty()) 
	{
		auto msg = static_cast<std::string>(m_textInput->GetText());

		AddMessage(msg);
		Engine::GetCommandExecutor()->CheckAndExecute(this, msg);
		m_textInput->SetText("");

		return;
	}

	m_textInput->UpdateByKeyboard(previous, current);
}
