#pragma once

#include "ICommandQueue.h"

struct VkQueue_T;
typedef VkQueue_T* VkQueue;

namespace OSK::GRAPHICS {

	class GpuVulkan;

	/// <summary>
	/// Una cola de comandos almacena todas las listas de comandos que se envían a la GPU.
	/// 
	/// Implementación de la interfaz para el renderizador de Vulkan.
	/// </summary>
	class OSKAPI_CALL CommandQueueVulkan : public ICommandQueue {

	public:

		void Create(TSize nativeQueueIndex, const GpuVulkan& gpu);
		VkQueue GetQueue() const;
		uint32_t GetQueueIndex() const;

	private:

		VkQueue queue = 0;
		uint32_t queueIndex = 0;

	};

}
