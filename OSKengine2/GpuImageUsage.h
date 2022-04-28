#pragma once

#include "EnumFlags.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Uso que se le va a dar a la imagen.
	/// 
	/// @note Se pueden combinar varios de estos usos.
	/// </summary>
	enum class GpuImageUsage {

		/// <summary>
		/// Se usar� como imagen final para el renderizado 3D.
		/// Almacenar� la imagen de color final.
		/// </summary>
		COLOR = 1,

		/// <summary>
		/// Se usar� como imagen de profundidad para el renderizado 3D.
		/// </summary>
		DEPTH_STENCIL = 2,

		/// <summary>
		/// Se usar� como read-only desde los shaders.
		/// </summary>
		SAMPLED = 4,

		/// <summary>
		/// Se usar� para copiar esta imagen a otra.
		/// </summary>
		TRANSFER_SOURCE = 8,

		/// <summary>
		/// Se copiar�n los contenidos de otra imagen a esta.
		/// </summary>
		TRANSFER_DESTINATION = 16,

		/// <summary>
		/// Se usar� como un cubemap.
		/// </summary>
		CUBEMAP = 32

	};

}

OSK_FLAGS(OSK::GRAPHICS::GpuImageUsage);
