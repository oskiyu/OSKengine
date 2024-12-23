#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "ICommandQueue.h"
#include "DefineAs.h"
#include "VulkanTypedefs.h"

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


		/// @return Familia de colas a partir de la cual
		/// se obtuvo esta cola.
		const QueueFamily& GetFamily() const;

		/// @return Índice de la cola dentro de la familia de colas.
		UIndex32 GetQueueIndexInsideFamily() const;

		/// @return Cola nativa.
		VkQueue GetQueue() const;

	private:

		QueueFamily m_family;
		UIndex32 m_queueIndex;

		VkQueue m_queue = nullptr;

	};

}

#endif
