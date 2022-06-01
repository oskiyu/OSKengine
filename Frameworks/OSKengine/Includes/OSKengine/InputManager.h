#pragma once

#include "OSKmacros.h"
#include "DynamicArray.hpp"

namespace OSK::IO {

	class IInputListener;
	class Window;
	
	/// <summary>
	/// Maneja los input listeners del juego, llamando a sus funciones
	/// cuando sea necesario.
	/// </summary>
	class OSKAPI_CALL InputManager {

	public:

		/// <summary>
		/// Añade un listener para que sea manejado por el juego.
		/// </summary>
		/// 
		/// @note Si el listener ya ha sido añadido, no ocurre nada.
		void AddListener(IInputListener* listener);

		/// <summary>
		/// Quita un listener para que ya no sea manejado por el juego.
		/// </summary>
		/// 
		/// @note Si el listener no está dentro del manager, no ocurre nada.
		void RemoveListener(IInputListener* listener);

		/// <summary>
		/// Comprueba el estado del input, llamando a las funciones
		/// de los listeneres cuando sea necesario.
		/// </summary>
		void _Update(const Window& window);

	private:

		DynamicArray<IInputListener*> listeners;

	};

}
