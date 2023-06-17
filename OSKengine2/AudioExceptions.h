#pragma once

#include "EngineException.h"
#include <string>
#include <AL/al.h>

namespace OSK {

	static std::string GetErrorDesc(ALCenum error) {
		switch (error) {
			case AL_INVALID_NAME: return "AL_INVALID_NAME";
			case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
			case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
			case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION";
			case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";

			default: return std::to_string(error);
		}
	}

	class OSKAPI_CALL AudioDeviceCreationException : public EngineException {

	public:

		AudioDeviceCreationException(
			std::string_view msg,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(std::format("AudioDeviceCreationException: No se ha creado correctamente el device: {}.", msg)) { }

	};

	class OSKAPI_CALL AudioException : public EngineException {

	public:

		AudioException(
			std::string_view msg,
			ALCenum code,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(std::format("AudioException: {}. Code: {}.", msg, GetErrorDesc(code))) { }

	};

}
