#pragma once

#include "EnumFlags.hpp"

namespace OSK::GRAPHICS {

	/// @brief Tipos de comandos que puede soportar
	/// una cola de comandos.
	enum class CommandQueueSupport {

		/// @brief Comandos gr�ficos.
		GRAPHICS,
		/// @brief Comandos de computaci�n gene�rica.
		COMPUTE,
		/// @brief Comandos de transferencia de recursos.
		TRANSFER,
		/// @brief Comandos de presentaci�n en pantalla.
		PRESENTATION

	};

}

OSK_FLAGS(OSK::GRAPHICS::CommandQueueSupport);
