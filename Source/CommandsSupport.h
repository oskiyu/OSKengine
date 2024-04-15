#pragma once

#include "EnumFlags.hpp"


namespace OSK::GRAPHICS {

	/// @brief Tipos de comandos soportados por
	/// una cola de comandos y / o una lista de comandos.
	enum class CommandsSupport {

		/// @brief Comandos de renderizado.
		GRAPHICS = 1 << 0,

		/// @brief Comandos de computación genérica.
		COMPUTE = 1 << 1,

		/// @brief Comandos de transferencia.
		TRANSFER = 1 << 2,

		/// @brief Comandos de presentación a pantalla.
		PRESENTATION = 1 << 3

	};

}

OSK_FLAGS(OSK::GRAPHICS::CommandsSupport);
