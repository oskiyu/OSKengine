#pragma once

#include "GameObject.h"
#include "EngineException.h"

namespace OSK::ECS {

	/// @brief Excepción que ocurre cuando se trata de operar sobre un objeto que no existe.
	class InvalidObjectException : public EngineException {

	public:

		/// @brief Excepción que ocurre cuando se trata de registrar
		/// un componente que ya ha sido previamente registrado.
		/// @param componentName Nombre del componente.
		explicit InvalidObjectException(
			GameObjectIndex obj,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("InvalidObjectException: El objeto {} no existe.", obj),
				location) { }

	};

	/// @brief Excepción que se da cuando se trata de operar sobre un componente de
	/// un objeto que no tiene dicho componente.
	class ComponentNotFoundException : public EngineException {

	public:

		/// @brief Excepción que se da cuando se trata de operar sobre un componente de
		/// un objeto que no tiene dicho componente.
		/// @param componentName Nombre del componente.
		/// @param obj ID del objeto que debería tener el componente.
		ComponentNotFoundException(
			std::string_view componentName,
			GameObjectIndex obj,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("El objeto {} no contiene el componente {}.", obj, componentName),
				location) {}

	};

	/// @brief Excepción que ocurre cuando se trata de registrar
	/// un componente que ya ha sido previamente registrado.
	class ComponentNotRegisteredException : public EngineException {

	public:

		/// @param componentName Nombre del componente.
		explicit ComponentNotRegisteredException(
			std::string_view componentName,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("El componente {} no ha sido registrado.", componentName),
				location) { }

	};

	/// @brief Excepción que ocurre cuando se trata de añadir un componente a un objeto
	/// que ya tiene un componente del tipo dado.
	/// @see ErrorCode::ECS_0002_OBJECT_ALREADY_HAS_COMPONENT.
	class ObjectAlreadyHasComponentException : public EngineException {

	public:

		/// @brief Excepción que ocurre cuando se trata de añadir un componente a un objeto
		/// que ya tiene un componente del tipo dado.
		/// @param componentName Nombre del componente.
		/// @param object ID del objeto que ya tiene el componente.
		ObjectAlreadyHasComponentException(
			std::string_view componentName,
			GameObjectIndex object,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("El objeto {} ya contiene un componente del tipo {}.", object, componentName),
				location) {}

	};

	/// @brief Excepción que ocurre cuando se trata de registrar
	/// un componente que ya ha sido previamente registrado.
	/// @see ErrorCode::ECS_0001_COMPONENT_ALREADY_REGISTERED.
	class ComponentAlreadyRegisteredException : public EngineException {

	public:

		/// @brief Excepción que ocurre cuando se trata de registrar
		/// un componente que ya ha sido previamente registrado.
		/// @param componentName Nombre del componente.
		explicit ComponentAlreadyRegisteredException(
			std::string_view componentName,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("El componente {} ya ha sido registrado.", componentName),
				location) { }

	};

	/// @brief Excepción que se da cuando se trata de operar sobre un sistema que no
	/// ha sido registrado.
	class SystemNotFoundException : public EngineException {

	public:

		SystemNotFoundException(
			std::string_view systemName,
			const std::source_location& location = std::source_location::current())
			:
			EngineException(
				std::format("SystemNotFoundException: El sistema {} no está registrado.", systemName),
				location) {}

	};

}
