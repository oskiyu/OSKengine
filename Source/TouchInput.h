#pragma once

#include "ApiCall.h"
#include "Vector2.hpp"
#include "NumericTypes.h"
#include "Uuid.h"

namespace OSK::IO {

	/// @brief UIID para diferenciar un toque
	/// de pantalla de otro.
	/// 
	/// Importante para procesar arrastres.
	using TouchInputUuid = BaseUuid<class TouchInputUuidTag>;

	/// @brief Clasificación de un toque
	/// en pantalla táctil, según acabe de
	/// empezar o represente un toque
	/// mantenido o de arrastre.
	enum class TouchInputType {
		START,
		END,
		CONTINUING
	};


	/// @brief Evento de entrada
	/// de pantalla táctil.
	struct TouchInput {

		/// @brief En píxeles de pantalla.
		Vector2f position = Vector2f::Zero;

		TouchInputType type = TouchInputType::START;
		
		/// @brief Tiempo, en segundos, desde que
		/// se comenzó a pulsar (para eventos 
		/// TouchInputType::END y TouchInputType::CONTINUING).
		TDeltaTime timeSinceInputStart = 0.0f;

		/// @brief ID del evento, para relacionarlos con
		/// eventos de entrada anteriores.
		TouchInputUuid inputId = TouchInputUuid::CreateEmpty();

	};

}
