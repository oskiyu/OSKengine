#pragma once

#include "ICommandQueue.h"

struct VkQueue_T;
typedef VkQueue_T* VkQueue;

namespace OSK {

	/// <summary>
	/// Una cola de comandos almacena todas las listas de comandos que se env�an a la GPU.
	/// 
	/// Implementaci�n de la interfaz para el renderizador de Vulkan.
	/// </summary>
	class OSKAPI_CALL CommandQueueVulkan : public ICommandQueue {

	public:

		void SetQueue(VkQueue q);
		VkQueue GetQueue() const;

	private:

		VkQueue queue;

	};

}
