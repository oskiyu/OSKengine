#pragma once

#include "EngineException.h"

namespace OSK::GRAPHICS {

	class GpuBufferCreationException : public EngineException {

	public:

		explicit GpuBufferCreationException(
			size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("GpuBufferCreationException: No se pudo crear el buffer. Code: {}.", code),
				location) { }

	};

	class GpuMemoryAllocException : public EngineException {

	public:

		explicit GpuMemoryAllocException(
			size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("GpuMemoryAllocException: No se pudo asignar memoria en la GPU. Code: {}.", code),
				location) { }

	};

	class NoCompatibleGpuMemoryException : public EngineException {

	public:

		explicit NoCompatibleGpuMemoryException(
			size_t code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("NoCompatibleGpuMemoryException: No encontró memoria compatible en la GPU. Code: {}.", code),
				location) { }

	};

	class GpuMemoryBlockNotEnoughSpaceException : public EngineException {

	public:

		explicit GpuMemoryBlockNotEnoughSpaceException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("GpuMemoryBlockNotEnoughSpaceException: Este bloque de memoria no tiene suficiente espacio."),
				location) { }

	};

	class GpuMemoryNotMappedException : public EngineException {

	public:

		explicit GpuMemoryNotMappedException(
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("GpuMemoryNotMappedException: La memoria debe estar mapeada antes de escribirse."),
				location) { }

	};

}
