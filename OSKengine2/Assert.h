#pragma once

#include "OSKmacros.h"

#include <format>


#ifndef OSK_ASSERT_2

/// @brief Comrpueba si una condici�n se cumple.
/// En caso de no cumplirse, lanza la excepci�n indicada,
/// muestra un mensaje por el logger indicado y
/// muestra una ventana con el mensaje de la excepci�n.
/// @param condition Condici�n que debe cumplirse (debe ser verdadera).
/// @param exception Excepci�n que se lanzar� si la condici�n no se cumple.
/// @param logger Logger por el que se escribir� el mensaje de la excepci�n.
/// Si es `nullptr`, no loggear� ning�n mensaje.
#define OSK_ASSERT_2(condition, exception, logger)		\
if (!(condition)) {										\
	const auto exceptionInstance = (exception);\
	const std::string msg = std::format("Excepci�n producida:\n{}", exceptionInstance.what());	\
	if ((logger))											\
		logger->Log(OSK::IO::LogLevel::L_ERROR, msg);	\
	throw exceptionInstance;	\
}						\
//OSK::IO::Window::ShowMessageBox(msg);

#endif


#ifndef OSK_ASSERT

/// @brief Comrpueba si una condici�n se cumple.
/// En caso de no cumplirse, lanza la excepci�n indicada,
/// muestra un mensaje por el logger global y
/// muestra una ventana con el mensaje de la excepci�n.
/// @param condition Condici�n que debe cumplirse (debe ser verdadera).
/// @param exception Excepci�n que se lanzar� si la condici�n no se cumple.
/// 
/// @note Si @code Engine::GetLogger() @endcode es `nullptr`, no loggear� ning�n mensaje.
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
