#pragma once

#include "Vector4.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Las coordenadas de texturas pueden ser de dos tipos:
	/// - Normalizadas (0 - 1).
	/// - P�xeles (depende del tama�o de la imagen).
	/// </summary>
	enum class TextureCoordsType {

		/// <summary>
		/// Las coordenadas est�n dentro del rango 0 - 1.
		/// Permite seleccionar zonas independientemente de
		/// la resolucion de la imagen.
		/// </summary>
		NORMALIZED,

		/// <summary>
		/// Las coordenadas est�n especificadas en p�xeles.
		/// </summary>
		PIXELS

	};

	/// <summary>
	/// Contiene las coordenadas de texturas y el tipo que son
	/// (normalizadas o por p�xeles.
	/// </summary>
	struct TextureCoordinates2D {

		Vector4f texCoords;
		TextureCoordsType type;

		/// <summary>
		/// Crea las coordenadas normalizadas.
		/// </summary>
		static TextureCoordinates2D Normalized(const Vector4f texCoords) {
			return { texCoords, TextureCoordsType::NORMALIZED };
		}

		/// <summary>
		/// Crea las coordenadas por p�xeles.
		/// </summary>
		static TextureCoordinates2D Pixels(const Vector4f texCoords) {
			return { texCoords, TextureCoordsType::PIXELS };
		}

		Vector4f GetNormalized(const Vector2f& imageSize) const;

	};

}
