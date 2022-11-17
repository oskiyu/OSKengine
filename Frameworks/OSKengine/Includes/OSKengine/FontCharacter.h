#pragma once

#include "Vector4.hpp"
#include "OSKmacros.h"

namespace OSK::ASSETS {

	/// <summary>
	/// Información básica de un carácter de una 
	/// instancia de fuente en concreto.
	/// </summary>
	/// 
	/// @warning Únicamente válido para un caracter con un tamaño de fuente específico.
	struct FontCharacter {

		/// <summary> Coordenadas de texturas del sprite. </summary>
		/// 
		/// @note En píxeles.
		Vector4i texCoords;

		/// <summary> Tamaño del sprite. </summary>
		///
		/// @note En píxeles.
		Vector2f size;

		/// <summary>  Espaciado del carácter. </summary>
		///
		/// @note En píxeles.
		Vector2f bearing;

		/// <summary> Espaciado horizontal del carácter. </summary>
		///
		/// @note En píxeles.
		TSize advance = 0;

	};

};
