#pragma once

#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Modo de sincronizaci�n de los frames.
	/// </summary>
	enum class OSKAPI_CALL PresentMode {

		/// <summary>
		/// Sincorinizaci�n vertical.
		/// Max FPS: max FPS del monitor.
		/// </summary>
		VSYNC,

		/// <summary>
		/// Sincornizaci�n vertical de triple buffer.
		/// Max FPS: max FPS del monitor.
		/// Aunque no tenga m�s FPS, se renderizan m�s frames que no son renderizados.
		/// </summary>
		VSYNC_TRIPLE_BUFFER,

		/// <summary>
		/// No syc (m�ximos FPS posibles, con tearing).
		/// </summary>
		INMEDIATE

	};

}
