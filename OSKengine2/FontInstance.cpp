#include "FontInstance.h"

#include "IGpuImage.h"
#include "FontCharacter.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

constexpr TSize SPACES_PER_TAB = 4;

Vector2f FontInstance::GetTextSize(const std::string& string) const {
	//Char que se tomará como referencia en cuanto al tamaño de las letras.
	const FontCharacter& reference = characters.Get('A');
	std::string::const_iterator iterador;

	if (string == "")
		return Vector2(0.0f);

	float temporalSizeX = 0.0f; // Tamaño de la linea procesada en un momento dado.
	float sizeX = 0.0f;
	float sizeY = reference.bearing.Y * fontSize + reference.size.Y * fontSize;

	for (iterador = string.begin(); iterador != string.end(); iterador++) {
		const FontCharacter& character = characters.Get(*iterador);

		if (*iterador == '\n') {
			if (temporalSizeX > sizeX) {
				sizeX = temporalSizeX;
				temporalSizeX = 0;

				sizeY += reference.bearing.Y * fontSize + reference.size.Y * fontSize;
			}
		}
		if (*iterador == '\t') {
			temporalSizeX += (character.bearing.X * fontSize + character.size.X * fontSize) * SPACES_PER_TAB;
		}

		if (*iterador == ' ')
			temporalSizeX += (reference.bearing.X * fontSize + reference.size.X * fontSize);

		temporalSizeX += character.bearing.X * fontSize + character.size.X * fontSize;
	}

	if (temporalSizeX > sizeX)
		sizeX = temporalSizeX;

	return Vector2f(sizeX, sizeY);
}
