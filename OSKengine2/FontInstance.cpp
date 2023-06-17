#include "FontInstance.h"

#include "IGpuImage.h"
#include "FontCharacter.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

constexpr USize32 SPACES_PER_TAB = 4;

Vector2f FontInstance::GetTextSize(std::string_view string) const {
	//Char que se tomará como referencia en cuanto al tamaño de las letras.
	const FontCharacter& reference = characters.Get('A');

	if (string == "")
		return Vector2f::Zero;

	float temporalSizeX = 0.0f; // Tamaño de la linea procesada en un momento dado.
	float sizeX = 0.0f;
	float sizeY = reference.bearing.y * fontSize + reference.size.y * fontSize;

	for (auto iterador = string.begin(); iterador != string.end(); iterador++) {
		const FontCharacter& character = characters.Get(*iterador);

		if (*iterador == '\n') {
			if (temporalSizeX > sizeX) {
				sizeX = temporalSizeX;
				temporalSizeX = 0;

				sizeY += reference.bearing.y * fontSize + reference.size.y * fontSize;
			}
		}
		if (*iterador == '\t') {
			temporalSizeX += (character.bearing.x * fontSize + character.size.x * fontSize) * SPACES_PER_TAB;
		}

		if (*iterador == ' ')
			temporalSizeX += (reference.bearing.x * fontSize + reference.size.x * fontSize);

		temporalSizeX += character.bearing.x * fontSize + character.size.x * fontSize;
	}

	if (temporalSizeX > sizeX)
		sizeX = temporalSizeX;

	return Vector2f(sizeX, sizeY);
}
