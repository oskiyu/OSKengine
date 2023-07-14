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
	this->fuente = font;
}

void Console::WriteLine(const std::string& text) {
	const Entry entry {
		.text = text,
		.timestamp = Engine::GetCurrentTime()
	};

	entries.Insert(entry);
}

void Console::Draw(SpriteRenderer* spriteRenderer) {
	Vector2f position = Vector2f(10.0f, 10.0f);

	Transform2D transform(EMPTY_GAME_OBJECT);
	transform.SetPosition(position);

	for (const auto& e : entries) {
		spriteRenderer->DrawString(*fuente, fontSize, e.text, transform, Color::White);
		transform.AddPosition(Vector2f(0, (float)fontSize + 2.0f));
	}
}
