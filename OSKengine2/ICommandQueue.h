#pragma once

#include "OSKmacros.h"
#include <type_traits>

namespace OSK {

	/// <summary>
	/// Una cola de comandos almacena todas las listas de comandos que se envían a la GPU.
	/// 
	/// Esta interfaz no implementa ningún código, se debe usar una instancia de
	/// CommandQueueVulkan o CommandQueueDx12.
	/// </summary>
	class OSKAPI_CALL ICommandQueue {

	public:

		virtual ~ICommandQueue() = default;

		template <typename T> T* As() const requires std::is_base_of_v<ICommandQueue, T> {
			return (T*)this;
		}

	};

}
