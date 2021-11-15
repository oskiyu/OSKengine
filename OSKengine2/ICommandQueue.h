#pragma once

#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Una cola de comandos almacena todas las listas de comandos que se env�an a la GPU.
	/// 
	/// Esta interfaz no implementa ning�n c�digo, se debe usar una instancia de
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
