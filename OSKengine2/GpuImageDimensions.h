#pragma once

namespace OSK::GRAPHICS {

	/// <summary>
	/// Dimensiones posibles de una imagen.
	/// </summary>
	enum class GpuImageDimension {

		/// <summary>
		/// Una única dimensión.
		/// </summary>
		d1D = 1,

		/// <summary>
		/// Imagen 2D.
		/// </summary>
		d2D,

		/// <summary>
		/// Imagen 3D.
		/// No es lo mismo que un cubemap, que es un 
		/// array de imágenes 2D.
		/// </summary>
		d3D

	};

}
