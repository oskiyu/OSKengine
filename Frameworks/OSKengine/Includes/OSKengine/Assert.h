#pragma once

#include "OSKmacros.h"

#include <string>

namespace OSK {

	/// <summary>
	/// Comprueba si la condición se cumple.
	/// Si no se cumple, muestra un mensaje de error 
	/// y throwea un error.
	/// </summary>
	/// <param name="condition">Condición que debe ser verdadera.</param>
	/// <param name="message">Mensaje a mostrar, si la condición es falsa.</param>
	/// <param name="functionName">Nombre de la función en la que se hace el assert.</param>
	/// <param name="line">Línea del archivo en la que se hace el assert.</param>
	/// <param name="file">Archivo en el que se hace el assert.</param>
	void OSKAPI_CALL RuntimeAssertIsTrueFunction(bool condition, const std::string& message, const std::string& functionName, uint32_t line, const std::string& file);

	/// <summary>
	/// Comprueba si la condición se cumple.
	/// Si no se cumple, muestra un mensaje de aviso.
	/// </summary>
	/// <param name="condition">Condición que debe ser verdadera.</param>
	/// <param name="message">Mensaje a mostrar, si la condición es falsa.</param>
	/// <param name="functionName">Nombre de la función en la que se hace el check.</param>
	/// <param name="line">Línea del archivo en la que se hace el check.</param>
	/// <param name="file">Archivo en el que se hace el check.</param>
	void OSKAPI_CALL RuntimeCheckIsTrueFunction(bool condition, const std::string& message, const std::string& functionName, uint32_t line, const std::string& file);

	/// <summary>
	/// Comprueba si la condición NO se cumple.
	/// Si se cumple, muestra un mensaje de error 
	/// y throwea un error.
	/// </summary>
	/// <param name="condition">Condición que debe ser falsa.</param>
	/// <param name="message">Mensaje a mostrar, si la condición es verdadera.</param>
	/// <param name="functionName">Nombre de la función en la que se hace el assert.</param>
	/// <param name="line">Línea del archivo en la que se hace el assert.</param>
	/// <param name="file">Archivo en el que se hace el assert.</param>
	void OSKAPI_CALL RuntimeAssertIsFalseFunction(bool condition, const std::string& message, const std::string& functionName, uint32_t line, const std::string& file);

	/// <summary>
	/// Comprueba si la condición NO se cumple.
	/// Si se cumple, muestra un mensaje de aviso.
	/// </summary>
	/// <param name="condition">Condición que debe ser falsa.</param>
	/// <param name="message">Mensaje a mostrar, si la condición es verdadera.</param>
	/// <param name="functionName">Nombre de la función en la que se hace el check.</param>
	/// <param name="line">Línea del archivo en la que se hace el check.</param>
	/// <param name="file">Archivo en el que se hace el check.</param>
	void OSKAPI_CALL RuntimeCheckIsFalseFunction(bool condition, const std::string& message, const std::string& functionName, uint32_t line, const std::string& file);

}

/// <summary>
/// Comprueba si la condición se cumple.
/// Si no se cumple, muestra un mensaje de error 
/// y throwea un error.
/// </summary>
/// <param name="condition">Condición que debe ser verdadera.</param>
/// <param name="msg">Mensaje a mostrar, si la condición es falsa.</param>
#define OSK_ASSERT(condition, msg) OSK::RuntimeAssertIsTrueFunction(condition, msg, __FUNCTION__, __LINE__, __FILE__)

/// <summary>
/// Comprueba si la condición se cumple.
/// Si no se cumple, muestra un mensaje de aviso.
/// </summary>
/// <param name="condition">Condición que debe ser verdadera.</param>
/// <param name="msg">Mensaje a mostrar, si la condición es falsa.</param>
#define OSK_CHECK(condition, msg) OSK::RuntimeCheckIsTrueFunction(condition, msg, __FUNCTION__, __LINE__, __FILE__)

/// <summary>
/// Comprueba si la condición NO se cumple.
/// Si se cumple, muestra un mensaje de error 
/// y throwea un error.
/// </summary>
/// <param name="condition">Condición que debe ser falsa.</param>
/// <param name="msg">Mensaje a mostrar, si la condición es verdadera.</param>
#define OSK_ASSERT_FALSE(condition, msg) OSK::RuntimeAssertIsFalseFunction(condition, msg, __FUNCTION__, __LINE__, __FILE__)

/// <summary>
/// Comprueba si la condición NO se cumple.
/// Si se cumple, muestra un mensaje de aviso.
/// </summary>
/// <param name="condition">Condición que debe ser falsa.</param>
/// <param name="msg">Mensaje a mostrar, si la condición es verdadera.</param>
#define OSK_CHECK_FALSE(condition, msg) OSK::RuntimeCheckIsFalseFunction(condition, msg, __FUNCTION__, __LINE__, __FILE__)
