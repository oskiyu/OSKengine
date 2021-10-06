#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "OSKtypes.h"

#include <cstdint>

namespace OSK {

	/// <summary>
	/// Representa un heightmap:
	/// una imagen 2D, en la que los p�xeles representan la altura de un punto en el terreno.
	/// Los puntos m�s bajos est�n en 0, y los m�s altos en 255.
	/// </summary>
	struct OSKAPI_CALL Heightmap {

		/// <summary>
		/// Destruye el heightmap y su informaci�n.
		/// </summary>
		~Heightmap();

		/// <summary>
		/// P�xeles del heightmap.
		/// </summary>
		OwnedPtr<uint8_t> data = nullptr;

		/// <summary>
		/// Tama�o (en p�xeles) de la imagen del heightmap.
		/// </summary>
		Vector2ui size = { 0, 0 };

	};

}
