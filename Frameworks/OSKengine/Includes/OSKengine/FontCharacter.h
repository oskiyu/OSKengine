#pragma once

#include "Vector4.hpp"
#include "OSKmacros.h"

namespace OSK::ASSETS {

	/// <summary>
	/// Informaci�n b�sica de un car�cter de una 
	/// instancia de fuente en concreto.
	/// </summary>
	/// 
	/// @warning �nicamente v�lido para un caracter con un tama�o de fuente espec�fico.
	struct FontCharacter {

		/// <summary> Coordenadas de texturas del sprite. </summary>
		/// 
		/// @note En p�xeles.
		Vector4i texCoords;

		/// <summary> Tama�o del sprite. </summary>
		///
		/// @note En p�xeles.
		Vector2f size;

		/// <summary>  Espaciado del car�cter. </summary>
		///
		/// @note En p�xeles.
		Vector2f bearing;

		/// <summary> Espaciado horizontal del car�cter. </summary>
		///
		/// @note En p�xeles.
		TSize advance = 0;

	};

};
