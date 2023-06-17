#pragma once

#include "OSKmacros.h"

#include <format>


#ifndef OSK_ASSERT_2

/// @brief Comrpueba si una condición se cumple.
/// En caso de no cumplirse, lanza la excepción indicada,
/// muestra un mensaje por el logger indicado y
/// muestra una ventana con el mensaje de la excepción.
/// @param condition Condición que debe cumplirse (debe ser verdadera).
/// @param exception Excepción que se lanzará si la condición no se cumple.
/// @param logger Logger por el que se escribirá el mensaje de la excepción.
/// Si es `nullptr`, no loggeará ningún mensaje.
#define OSK_ASSERT_2(condition, exception, logger)		\
if (!(condition)) {										\
	const auto exceptionInstance = (exception);\
	const std::string msg = std::format("Excepción producida:\n{}", exceptionInstance.what());	\
	if ((logger))											\
		logger->Log(OSK::IO::LogLevel::L_ERROR, msg);	\
	throw exceptionInstance;	\
}						\
//OSK::IO::Window::ShowMessageBox(msg);

#endif


#ifndef OSK_ASSERT

/// @brief Comrpueba si una condición se cumple.
/// En caso de no cumplirse, lanza la excepción indicada,
/// muestra un mensaje por el logger global y
/// muestra una ventana con el mensaje de la excepción.
/// @param condition Condición que debe cumplirse (debe ser verdadera).
/// @param exception Excepción que se lanzará si la condición no se cumple.
/// 
/// @note Si @code Engine::GetLogger() @endcode es `nullptr`, no loggeará ningún mensaje.
#define OSK_ASSERT(condition, exception) 	\
if (!(condition)) {										\
	const auto exceptionInstance = (exception);\
	throw exceptionInstance;	\
}						\

/*
	if ((OSK::Engine::GetLogger()))											\
		OSK::Engine::GetLogger()->Log(OSK::IO::LogLevel::L_ERROR, msg);	\
	OSK::IO::Window::ShowMessageBox(msg);	\
*/

#endif
