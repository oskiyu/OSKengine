#pragma once

#include "Vector4.hpp"
#include "OSKmacros.h"

namespace OSK::ASSETS {

	/// <summary>
	/// Informaci�n b�sica de un car�cter de una 
	/// instancia de fuente en concreto.
	/// 
	/// @warning �nicamente v�lido para un caracter con un tama�o de fuente espec�fico.
	/// </summary>
	struct FontCharacter {

		/// <summary>
		/// Coordenadas de texturas del sprite.
		/// 
		/// @note En p�xeles.
		/// </summary>
		Vector4i texCoords;

		/// <summary>
		/// Tama�o del sprite.
		/// </summary>
		Vector2f size;

		/// <summary>
		/// Espaciado del car�cter.
		/// </summary>
		Vector2f bearing;

		/// <summary>
		/// Espaciado horizontal del car�cter.
		/// </summary>
		TSize advance = 0;

	};

};
