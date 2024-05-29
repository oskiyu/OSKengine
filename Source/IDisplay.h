// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include "Vector2.hpp"
#include "Event.h"
#include "IQueryInterface.h"

#include <string>

namespace OSK::IO {

	/// @brief Interfaz que representa un display: la pantalla sobre la
	/// que se renderiza.
	/// 
	/// Toda clase derivada puede tambi�n implementar estas otras interfaces:
	/// - IFullscreenableDisplay.
	class OSKAPI_CALL IDisplay : public IQueryInterface {

	public:

		/// @brief Evento que se lanza cuando cambia la resoluci�n del display.
		struct ResolutionChangedEvent {
			OSK_EVENT("OSK::ResolutionChangedEvent");

			/// @brief Nueva resoluci�n, en p�xeles.
			Vector2ui newResolution;
		};

	public:

		virtual ~IDisplay() = default;

		OSK_DEFINE_AS(IDisplay)


		/// @brief Crea el display con los argumentos dados.
		/// @param resolution Resoluci�n inicial, en p�xeles.
		/// @param title T�tulo del display, si se aplica.
		virtual void Create(Vector2ui resolution, const std::string& title) = 0;

		/// @brief Realiza cualquier funci�n de actualizaci�n
		/// que necesite el display.
		virtual void Update() = 0;

		/// @brief Cierra el display.
		virtual void Close() = 0;


		/// @return True si el display est� activo.
		bool IsOpen() const;


		/// @return Resoluci�n actual, en p�xeles.
		Vector2ui GetResolution() const;

		/// @return Resoluci�n.X / Resoluci�n.Y.
		/// @note Si Resoluci�n.Y es 0, devuelve 1.
		float GetScreenRatio() const;

		/// @return Tasa de fotogramas por segundo soportado por
		/// el display.
		USize32 GetRefreshRate() const;

	protected:

		Vector2ui resolution = Vector2ui::Zero;
		USize32 refreshRate = 60;
		bool isOpen = false;

	};

}
