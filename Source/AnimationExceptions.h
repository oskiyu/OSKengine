#pragma once

#include "GameObject.h"
#include "EngineException.h"

namespace OSK::GRAPHICS {

	/// @brief Excepción que ocurre cuando se trata de añadir una animación que ya existe.
	class ModelAnimationNotFoundException : public EngineException {

	public:

		explicit ModelAnimationNotFoundException(
			std::string_view name,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("ModelAnimationNotFoundException: La animación {} no existe.", name),
				location) { }

	};

	/// @brief Excepción que ocurre cuando se trata de acceder a un skin que no existe.
	class ModelSkinNotFoundException : public EngineException {

	public:

		explicit ModelSkinNotFoundException(
			std::string_view name,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("ModelSkinNotFoundException: La skin {} no existe.", name),
				location) { }

	};

	/// @brief Excepción que ocurre cuando se trata de acceder a un bone que no existe.
	class BoneNotFoundException : public EngineException {

	public:

		explicit BoneNotFoundException(
			UIndex32 index,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("BoneNotFoundException: El bone con id {} no existe.", index),
				location) { }

		explicit BoneNotFoundException(
			std::string_view name,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("BoneNotFoundException: El bone {} no existe.", name),
				location) { }

	};

	/// @brief Excepción que ocurre cuando se trata de añadir un hueso que ya existe.
	class BoneAlreadyAddedException : public EngineException {

	public:

		explicit BoneAlreadyAddedException(
			UIndex32 index,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("BoneAlreadyAddedException: El hueso {} ya existe.", index),
				location) { }

	};

}
