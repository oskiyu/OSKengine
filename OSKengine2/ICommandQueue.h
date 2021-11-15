#pragma once

#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Una cola de comandos almacena todas las listas de comandos que se envían a la GPU.
	/// 
	/// Esta interfaz no implementa ningún código, se debe usar una instancia de
	/// CommandQueueVulkan o CommandQueueDx12.
	/// </summary>
	class OSKAPI_CALL ICommandQueue {

	public:

		~ICommandQueue() = default;

		template <typename T> T* As() const {
			return (T*)this;
		}

	};

}
