#pragma once

#include "EnumFlags.hpp"

namespace OSK::GRAPHICS {

	/// @brief Tipos de comandos que puede soportar
	/// una cola de comandos.
	enum class CommandQueueSupport {

		/// @brief Comandos gráficos.
		GRAPHICS,
		/// @brief Comandos de computación geneérica.
		COMPUTE,
		/// @brief Comandos de transferencia de recursos.
		TRANSFER,
		/// @brief Comandos de presentación en pantalla.
		PRESENTATION

	};

}

OSK_FLAGS(OSK::GRAPHICS::CommandQueueSupport);
