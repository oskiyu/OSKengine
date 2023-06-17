#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "GpuMemoryUsageInfo.h"

#include <type_traits>

namespace OSK::GRAPHICS {

	class ICommandPool;
	
	/// @brief Representa una conexi�n con la GPU.
	class OSKAPI_CALL IGpu {

	public:

		virtual ~IGpu() = default;

		OSK_DEFINE_AS(IGpu)

		/// @brief Crea una pool de comandos, para poder crear listas de comandos gr�ficos y de presentaci�n.
		/// @return Command pool para comandos gr�ficos y de presentaci�n.
		/// @throws CommandPoolCreationException Si ocurre alg�n error. 
		virtual OwnedPtr<class ICommandPool> CreateGraphicsCommandPool() = 0;

		/// @brief Crea una pool de comandos, para poder crear listas de comandos de computaci�n.
		/// @return Command pool para comandos de computaci�n.
		/// @throws CommandPoolCreationException Si ocurre alg�n error. 
		virtual OwnedPtr<ICommandPool> CreateComputeCommandPool() = 0;

		/// @brief Obtiene la informaci�n sobre el uso de memoria de esta GPU,
		/// incluyendo espacio usado y espacio disponible.
		/// @return Informaci�n sobre el uso de memoria de esta GPU.
		virtual GpuMemoryUsageInfo GetMemoryUsageInfo() const = 0;


		/// @brief Cierra la conexi�n con la GPU.
		virtual void Close() = 0;

	};

}
