#pragma once

#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Modo de sincronización de los frames.
	/// </summary>
	enum class OSKAPI_CALL PresentMode {

		/// <summary>
		/// Sincorinización vertical.
		/// Max FPS: max FPS del monitor.
		/// </summary>
		VSYNC,

		/// <summary>
		/// Sincornización vertical de triple buffer.
		/// Max FPS: max FPS del monitor.
		/// Aunque no tenga más FPS, se renderizan más frames que no son renderizados.
		/// </summary>
		VSYNC_TRIPLE_BUFFER,

		/// <summary>
		/// No syc (máximos FPS posibles, con tearing).
		/// </summary>
		INMEDIATE

	};

}
