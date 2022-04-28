#pragma once

#include "Vector4.hpp"
#include "OSKmacros.h"

namespace OSK::ASSETS {

	/// <summary>
	/// Información básica de un carácter de una 
	/// instancia de fuente en concreto.
	/// 
	/// @warning Únicamente válido para un caracter con un tamaño de fuente específico.
	/// </summary>
	struct FontCharacter {

		/// <summary>
		/// Coordenadas de texturas del sprite.
		/// 
		/// @note En píxeles.
		/// </summary>
		Vector4i texCoords;

		/// <summary>
		/// Tamaño del sprite.
		/// </summary>
		Vector2f size;

		/// <summary>
		/// Espaciado del carácter.
		/// </summary>
		Vector2f bearing;

		/// <summary>
		/// Espaciado horizontal del carácter.
		/// </summary>
		TSize advance = 0;

	};

};
