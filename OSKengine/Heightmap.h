#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "OSKtypes.h"

#include <cstdint>

namespace OSK {

	/// <summary>
	/// Representa un heightmap:
	/// una imagen 2D, en la que los píxeles representan la altura de un punto en el terreno.
	/// Los puntos más bajos están en 0, y los más altos en 255.
	/// </summary>
	struct OSKAPI_CALL Heightmap {

		/// <summary>
		/// Destruye el heightmap y su información.
		/// </summary>
		~Heightmap();

		/// <summary>
		/// Píxeles del heightmap.
		/// </summary>
		OwnedPtr<uint8_t> data = nullptr;

		/// <summary>
		/// Tamaño (en píxeles) de la imagen del heightmap.
		/// </summary>
		Vector2ui size = { 0, 0 };

	};

}
