#include "Font.h"

namespace OSK {

	Vector2 Font::GetTextSize(const std::string& texto, const Vector2& size) const {
		//Char que se tomará como referencia en cuanto al tamaño de las letras.
		FontChar reference = characters.at('A');
		std::string::const_iterator iterador;

		if (texto == "")
			return Vector2(0.0f);

		float_t temporalSizeX = 0.0f;
		float_t sizeX = 0.0f;
		float_t sizeY = reference.bearing.Y * size.Y + reference.size.Y * size.Y;

		for (iterador = texto.begin(); iterador != texto.end(); iterador++) {
			FontChar character = characters.at(*iterador);
			if (*iterador == '\n') {
				if (temporalSizeX > sizeX) {
					sizeX = temporalSizeX;
					temporalSizeX = 0;

					sizeY += reference.bearing.Y * size.Y + reference.size.Y * size.Y;
				}
			}
			if (*iterador == '\t') {
				temporalSizeX += (character.bearing.X * size.X + character.size.X * size.X) * SPACES_PER_TAB;
			}

			if (*iterador == ' ')
				temporalSizeX += (reference.bearing.X * size.X + reference.size.X * size.X);

			temporalSizeX += character.bearing.X * size.X + character.size.X * size.X;
		}

		if (temporalSizeX > sizeX)
			sizeX = temporalSizeX;

		return Vector2(sizeX, sizeY);
	}

}