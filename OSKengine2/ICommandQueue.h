#pragma once

#include "OSKmacros.h"
#include "CommandQueueSupport.h"

namespace OSK::GRAPHICS {

	
	/// @brief Representa un puerto de entrada para los comandos
	/// de la GPU.
	class OSKAPI_CALL ICommandQueue {

	public:

		virtual ~ICommandQueue() = default;

		OSK_DEFINE_AS(ICommandQueue);

	protected:

		inline ICommandQueue(CommandQueueSupport support) : supportType(support) {}

	private:

		CommandQueueSupport supportType;

	};

}
