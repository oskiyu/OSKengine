#include "ReservedText.h"

using namespace OSK;

ReservedText::ReservedText() {
	spriteContainer.number = 0;
	spriteContainer.shouldBeCleared = false;
}

ReservedText::~ReservedText() {
	spriteContainer.Clear(true);
}

void ReservedText::Reserve(const uint32_t& length) {
	spriteContainer.Clear(true);

	spriteContainer.number = length;
	if (length == 1)
		spriteContainer.sprites = new Sprite;
	else
		spriteContainer.sprites = new Sprite[length];
}

void ReservedText::SetText(const std::string& text) {
	const uint32_t length = text.length();

	if (length > spriteContainer.number)
		Reserve(length);

	this->text = text;
}

void ReservedText::Clear() {
	spriteContainer.Clear(true);
}