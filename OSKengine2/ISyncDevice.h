#pragma once

#include "OSKmacros.h"
#include <type_traits>

namespace OSK {

	/// <summary>
	/// Una estructura que se encarga de sincronizar el renderizador,
	/// asegurándose de que se renderiza sobre la imagen correcta del
	/// swapchain, y de que no comenzamos a renderizar hasta no tener
	/// disponible la siguiente imagen del swapchain.
	/// 
	/// Esta interfaz no tiene funciones comunes, ya que la implementación
	/// varía considerablemente dependiendo de la API gráfica usada.
	/// </summary>
	class OSKAPI_CALL ISyncDevice {

	public:

		virtual ~ISyncDevice() = default;

		template <typename T> T* As() const requires std::is_base_of_v<ISyncDevice, T> {
			return (T*)this;
		}

	};

}
