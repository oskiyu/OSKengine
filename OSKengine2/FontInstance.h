#pragma once

#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "HashMap.hpp"
#include "FontCharacter.h"

namespace OSK::ASSETS {

	/// <summary>
	/// Representa una fuente que ha sido generada con un tamaño específico.
	/// Cada tamaño de fuente tendrá su propia imagen cargada.
	/// </summary>
	struct FontInstance {

		/// <summary>
		/// Imagen en la GPU.
		/// 
		/// @note Nunca será null.
		/// </summary>
		UniquePtr<GRAPHICS::GpuImage> image;

		/// <summary>
		/// Información de los sprites del carácter.
		/// </summary>
		HashMap<char, FontCharacter> characters;

	};

}
