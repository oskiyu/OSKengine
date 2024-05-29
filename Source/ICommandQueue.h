#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include "CommandsSupport.h"
#include "QueueFamilyIndices.h"

namespace OSK::GRAPHICS {

	enum class GpuQueueType;
	
	/// @brief Representa un puerto de entrada para los comandos
	/// de la GPU.
	class OSKAPI_CALL ICommandQueue {

	public:

		virtual ~ICommandQueue() = default;

		OSK_DEFINE_AS(ICommandQueue);


		/// @return Tipos de comandos soportados 
		CommandsSupport GetSupportedCommands() const;

		/// @return Familia de colas a partir de la cual
		/// se obtuvo esta cola.
		const QueueFamily& GetFamily() const;

		/// @return Tipo de cola.
		GpuQueueType GetQueueType() const;

		/// @return Índice de la cola dentro de la familia de colas.
		UIndex32 GetQueueIndexInsideFamily() const;

	protected:

		ICommandQueue(
			QueueFamily family,
			GpuQueueType queueType,
			UIndex32 queueIndexInsideFamily);

	private:

		QueueFamily m_family;
		GpuQueueType m_mainQueueType;
		UIndex32 m_queueIndex;

	};

}
