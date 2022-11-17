// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"
#include "IQueryInterface.h"

#include "Vector2.hpp"

namespace OSK::IO {

	/// @brief Interfaz que representa un display que puede alternar
	/// entre pantalla completa y modo ventana.
	class OSKAPI_CALL IFullscreenableDisplay {

	public:

		virtual ~IFullscreenableDisplay() = default;

		OSK_DEFINE_IUUID((TInterfaceUuid)IUUID::IFullscreenableDisplay);

		/// @return True si el display se encuentra actualmente en pantalla completa.
		bool IsFullscreen() const;

		/// @brief Establece modo pantalla completa o modo ventana.
		/// @param fullscreen True: modo pantalla completa.
		/// False: modo ventana.
		virtual void SetFullscreen(bool fullscreen) = 0;

		/// @brief Cambia de modo pantalla completa a modo ventana o viceversa.
		void ToggleFullscreen();

	protected:

		/// @brief Información sobre el estado del display en modo ventana.
		/// Para poder recuperar su estado anterior después de haber pasado
		/// por el modo pantalla completa.
		struct PreviousDisplayState {
			/// @brief Resolución, en píxeles.
			Vector2i size;
			/// @brief Posición, en píxeles, respecto a la esquina superior izquierda.
			Vector2i position;
		} previous;

		bool isFullscreen = false;

	};

}
