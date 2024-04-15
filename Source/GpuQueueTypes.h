#pragma once

namespace OSK::GRAPHICS {

	/// @brief Tipos de colas GPU ofrecidas por
	/// el motor.
	enum class GpuQueueType {

		/// @brief Cola principal de renderizado.
		/// Soporta:
		/// - Operaciones gráficas (`CommandsSupport::GRAPHICS`).
		/// - Operaciones de computación (`CommandsSupport::COMPUTE`).
		/// - Operaciones de transferencia (`CommandsSupport::TRANSFER`).
		MAIN,

		/// @brief Cola de presentación.
		/// Soporta únicamente `CommandsSupport::PRESENTATION`.
		PRESENTATION,

		/// @brief Cola de transferencia asíncrona.
		/// Soporta únicamente `CommandsSupport::TRANSFER`.
		/// 
		/// @note Dependiendo de la GPU, es posible que se enlace con la
		/// cola principal, si la GPU no dispone de una cola exclusiva de transferencia.
		ASYNC_TRANSFER

	};

}
