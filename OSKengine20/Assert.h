#pragma once

#include <string>

import OSKengine.Assert;

/// <summary>
/// Comprueba si la condición se cumple.
/// Si no se cumple, muestra un mensaje de error 
/// y throwea un error.
/// </summary>
/// <param name="condition">Condición que debe ser verdadera.</param>
/// <param name="msg">Mensaje a mostrar, si la condición es falsa.</param>
#define OSK_ASSERT(condition, msg) OSK::RuntimeCheckIsTrueFunction(condition, msg, __FUNCTION__, __LINE__, __FILE__)

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
