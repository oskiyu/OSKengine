#pragma once

#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "HashMap.hpp"
#include "FontCharacter.h"

namespace OSK::ASSETS {

	/// <summary>
	/// Representa una fuente que ha sido generada con un tama�o espec�fico.
	/// Cada tama�o de fuente tendr� su propia imagen cargada.
	/// </summary>
	struct FontInstance {

		/// <summary>
		/// Imagen en la GPU.
		/// 
		/// @note Nunca ser� null.
		/// </summary>
		UniquePtr<GRAPHICS::GpuImage> image;

		/// <summary>
		/// Informaci�n de los sprites del car�cter.
		/// </summary>
		HashMap<char, FontCharacter> characters;

	};

}
