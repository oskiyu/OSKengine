#pragma once

#include "EnumFlags.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Define el layout actual de una imagen.
	/// El layout de una imagen en un momento dado debe ser
	/// el correcto para poder efectuar su tarea.
	/// Se pueden combinar varios de estos layouts.
	/// </summary>
	enum class GpuImageLayout {

		/// <summary>
		/// Sin definir / no nos importa.
		/// </summary>
		UNDEFINED,

		/// <summary>
		/// Imagen final del renderizado que será enviada
		/// al monitor.
		/// </summary>
		PRESENT,

		/// <summary>
		/// Se usará para renderizar en ella una escena 3D.
		/// </summary>
		COLOR_ATTACHMENT,

		/// <summary>
		/// Se usará para escribir datos de profundidad.
		/// </summary>
		DEPTH_STENCIL_TARGET,

		/// <summary>
		/// Se usará para leer datos de profundidad.
		/// </summary>
		DEPTH_STENCIL_READ_ONLY,

		/// <summary>
		/// Se usará desde los shaders (read-only).
		/// </summary>
		SHADER_READ_ONLY,

		/// <summary>
		/// Se usará para copiar sus datos a otra imagen.
		/// </summary>
		TRANSFER_SOURCE,

		/// <summary>
		/// Los contenidos de otra imagen se copiarán a esta.
		/// </summary>
		TRANSFER_DESTINATION

	};

}

OSK_FLAGS(OSK::GRAPHICS::GpuImageLayout);
