#pragma once

#include "ToString.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Define el modo de sincronizaci�n vertical.
	/// 
	/// define a qu� ritmo se actualiza el fotograma
	/// presentado en el monitor.
	/// </summary>
	enum class PresentMode {

		/// <summary>
		/// No se espera a que la imagen actual termine de presentarse
		/// para mostrar la nueva imagen.
		/// 
		/// Produce tearing.
		/// </summary>
		VSYNC_OFF,

		/// <summary>
		/// Espera a que la imagen actual termine de presentarse
		/// para mostrar la nueva imagen.
		/// 
		/// No produce tearing.
		/// </summary>
		VSYNC_ON,

		/// <summary>
		/// Espera a que la imagen actual termine de presentarse
		/// para mostrar la nueva imagen.
		/// 
		/// Se sigue procesando nuevas im�genes aunque todav�a no se puedan
		/// presentar, permitiendo presentar una imagen m�s reciente si lo
		/// comparas con PresentMode::VSYNC_ON.
		/// 
		/// No produce tearing.
		/// </summary>
		VSYNC_ON_TRIPLE_BUFFER

	};

}

template <> std::string OSK::ToString<OSK::GRAPHICS::PresentMode>(const OSK::GRAPHICS::PresentMode& mode);
