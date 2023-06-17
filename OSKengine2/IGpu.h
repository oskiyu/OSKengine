#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "GpuMemoryUsageInfo.h"

#include <type_traits>

namespace OSK::GRAPHICS {

	class ICommandPool;
	
	/// @brief Representa una conexión con la GPU.
	class OSKAPI_CALL IGpu {

	public:

		virtual ~IGpu() = default;

		OSK_DEFINE_AS(IGpu)

		/// @brief Crea una pool de comandos, para poder crear listas de comandos gráficos y de presentación.
		/// @return Command pool para comandos gráficos y de presentación.
		/// @throws CommandPoolCreationException Si ocurre algún error. 
		virtual OwnedPtr<class ICommandPool> CreateGraphicsCommandPool() = 0;

		/// @brief Crea una pool de comandos, para poder crear listas de comandos de computación.
		/// @return Command pool para comandos de computación.
		/// @throws CommandPoolCreationException Si ocurre algún error. 
		virtual OwnedPtr<ICommandPool> CreateComputeCommandPool() = 0;

		/// @brief Obtiene la información sobre el uso de memoria de esta GPU,
		/// incluyendo espacio usado y espacio disponible.
		/// @return Información sobre el uso de memoria de esta GPU.
		virtual GpuMemoryUsageInfo GetMemoryUsageInfo() const = 0;


		/// @brief Cierra la conexión con la GPU.
		virtual void Close() = 0;

	};

}
