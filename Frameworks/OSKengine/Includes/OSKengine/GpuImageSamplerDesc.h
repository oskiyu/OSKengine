#pragma once

namespace OSK::GRAPHICS {

	/// <summary>
	/// Tipo de filtro que se aplicar� al acceder a la imagen.
	/// </summary>
	enum class GpuImageFilteringType {

		/// <summary>
		/// Valor linealmente interpolado con los colindantes.
		/// </summary>
		LIENAR,

		/// <summary>
		/// El valor no se interpola.
		/// </summary>
		NEAREST,

		/// <summary>
		/// El valor se interpola c�bicamente con los colindantes.
		/// </summary>
		CUBIC
	};

	/// <summary>
	/// Describe qu� color se seleccionar� al acceder fuera de las
	/// coordenadas de la textura.
	/// </summary>
	enum class GpuImageAddressMode {

		/// <summary>
		/// La textura se repite infinitamente.
		/// </summary>
		REPEAT,

		/// <summary>
		/// La textura se repite infinitamente, d�ndose la vuelta.
		/// </summary>
		MIRRORED_REPEAT,

		/// <summary>
		/// Se devuelve el color del borde m�s cercano.
		/// </summary>
		EDGE,

		/// <summary>
		/// Borde negro.
		/// </summary>
		BACKGROUND

	};

	/// <summary>
	/// Establece propiedades b�sicas sobre el
	/// acceso a la imagen.
	/// </summary>
	struct GpuImageSamplerDesc {
		GpuImageFilteringType filteringType = GpuImageFilteringType::LIENAR;
		GpuImageAddressMode addressMode = GpuImageAddressMode::REPEAT;
	};

}
