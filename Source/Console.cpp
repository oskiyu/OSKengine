#include "Console.h"

#include "OSKengine.h"
#include "SpriteRenderer.h"
#include "Transform2D.h"

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void Console::SetFont(Font* font) {
	m_fuente = font;
}

void Console::WriteLine(const std::string& text) {
	const Entry entry {
		.text = text,
		.timestamp = Engine::GetCurrentTime()
	};

	m_entries.Insert(entry);
}

void Console::Draw(SpriteRenderer* spriteRenderer) {
	Vector2f position = Vector2f(20.0f, 280.0f);

	Transform2D transform(EMPTY_GAME_OBJECT);
	transform.SetPosition(position);

	for (long int i = static_cast<long int>(m_entries.GetSize()) - 1, j = 0; i >= 0 && j < 15; i--, j++) {
		spriteRenderer->DrawString(*m_fuente, m_fontSize, m_entries[i].text, transform, Color::White);
		transform.AddPosition(Vector2f(0, (float)m_fontSize + 2.0f));
	}
}
