#include "Font.h"

namespace OSK {

	Vector2 Font::GetTextSize(const std::string& texto, const Vector2& size) const {
		//Char que se tomará como referencia en cuanto al tamaño de las letras.
		FontChar reference = Characters.at('A');
		std::string::const_iterator iterador;

		if (texto == "")
			return Vector2(0.0f);

		float_t temporalSizeX = 0.0f;
		float_t sizeX = 0.0f;
		float_t sizeY = reference.Bearing.Y * size.Y + reference.Size.Y * size.Y;

		for (iterador = texto.begin(); iterador != texto.end(); iterador++) {
			FontChar character = Characters.at(*iterador);
			if (*iterador == '\n') {
				if (temporalSizeX > sizeX) {
					sizeX = temporalSizeX;
					temporalSizeX = 0;

					sizeY += reference.Bearing.Y * size.Y + reference.Size.Y * size.Y;
				}
			}
			if (*iterador == '\t') {
				temporalSizeX += (character.Bearing.X * size.X + character.Size.X * size.X) * SPACES_PER_TAB;
			}

			if (*iterador == ' ')
				temporalSizeX += (reference.Bearing.X * size.X + reference.Size.X * size.X);

			temporalSizeX += character.Bearing.X * size.X + character.Size.X * size.X;
		}

		if (temporalSizeX > sizeX)
			sizeX = temporalSizeX;

		return Vector2(sizeX, sizeY);
	}

}