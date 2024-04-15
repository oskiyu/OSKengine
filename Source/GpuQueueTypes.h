#pragma once

namespace OSK::GRAPHICS {

	/// @brief Tipos de colas GPU ofrecidas por
	/// el motor.
	enum class GpuQueueType {

		/// @brief Cola principal de renderizado.
		/// Soporta:
		/// - Operaciones gr�ficas (`CommandsSupport::GRAPHICS`).
		/// - Operaciones de computaci�n (`CommandsSupport::COMPUTE`).
		/// - Operaciones de transferencia (`CommandsSupport::TRANSFER`).
		MAIN,

		/// @brief Cola de presentaci�n.
		/// Soporta �nicamente `CommandsSupport::PRESENTATION`.
		PRESENTATION,

		/// @brief Cola de transferencia as�ncrona.
		/// Soporta �nicamente `CommandsSupport::TRANSFER`.
		/// 
		/// @note Dependiendo de la GPU, es posible que se enlace con la
		/// cola principal, si la GPU no dispone de una cola exclusiva de transferencia.
		ASYNC_TRANSFER

	};

}
