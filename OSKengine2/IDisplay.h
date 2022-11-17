// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "Vector2.hpp"
#include "IQueryInterface.h"

#include <string>

namespace OSK::IO {

	/// @brief Interfaz que representa un display: la pantalla sobre la
	/// que se renderiza.
	/// 
	/// Toda clase derivada puede también implementar estas otras interfaces:
	/// - IFullscreenableDisplay.
	class OSKAPI_CALL IDisplay : public IQueryInterface {

	public:

		virtual ~IDisplay() = default;

		OSK_DEFINE_AS(IDisplay);


		/// @brief Crea el display con los argumentos dados.
		/// @param resolution Resolución inicial, en píxeles.
		/// @param title Título del display, si se aplica.
		virtual void Create(Vector2ui resolution, const std::string& title) = 0;

		/// @brief Realiza cualquier función de actualización
		/// que necesite el display.
		virtual void Update() = 0;

		/// @brief Cierra el display.
		virtual void Close() = 0;


		/// @return True si el display está activo.
		bool IsOpen() const;


		/// @return Resolución actual, en píxeles.
		Vector2ui GetResolution() const;

		/// @return Resolución.X / Resolución.Y.
		/// @note Si Resolución.Y es 0, devuelve 1.
		float GetScreenRatio() const;

		/// @return Tasa de fotogramas por segundo soportado por
		/// el display.
		TSize GetRefreshRate() const;

	protected:

		Vector2ui resolution = 0;
		bool isOpen = false;
		TSize refreshRate = 60;

	};

}
