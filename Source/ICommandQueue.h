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
		virtual CommandsSupport GetSupportedCommands() const;

		/// @return Tipo de cola.
		GpuQueueType GetQueueType() const;

	protected:

		explicit ICommandQueue(GpuQueueType queueType);

	private:

		GpuQueueType m_queueType;

	};

}
