#pragma once

#include "ICommandQueue.h"

OSK_VULKAN_TYPEDEF(VkQueue);

namespace OSK::GRAPHICS {

	class GpuVk;

	class OSKAPI_CALL CommandQueueVk final : public ICommandQueue {

	public:

		/// @brief Obtiene la cola de comandos.
		/// @param family Familia a la que pertenece la cola.
		/// @param queueIndexInsideFamily Índice de la cola dentro de la familia.
		/// @param queueType Tipo de cola.
		/// @param gpu Gpu sobre la que se aloja la cola.
		CommandQueueVk(
			QueueFamily family,
			UIndex32 queueIndexInsideFamily,
			GpuQueueType queueType,
			const GpuVk& gpu);


		/// @return Cola nativa.
		VkQueue GetQueue() const;

	private:

		VkQueue m_queue = nullptr;

	};

}
