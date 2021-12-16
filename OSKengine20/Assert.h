#pragma once

#include <string>

import OSKengine.Assert;

/// <summary>
/// Comprueba si la condici�n se cumple.
/// Si no se cumple, muestra un mensaje de error 
/// y throwea un error.
/// </summary>
/// <param name="condition">Condici�n que debe ser verdadera.</param>
/// <param name="msg">Mensaje a mostrar, si la condici�n es falsa.</param>
#define OSK_ASSERT(condition, msg) OSK::RuntimeCheckIsTrueFunction(condition, msg, __FUNCTION__, __LINE__, __FILE__)

/// <summary>
/// Comprueba si la condici�n se cumple.
/// Si no se cumple, muestra un mensaje de aviso.
/// </summary>
/// <param name="condition">Condici�n que debe ser verdadera.</param>
/// <param name="msg">Mensaje a mostrar, si la condici�n es falsa.</param>
#define OSK_CHECK(condition, msg) OSK::RuntimeCheckIsTrueFunction(condition, msg, __FUNCTION__, __LINE__, __FILE__)

/// <summary>
/// Comprueba si la condici�n NO se cumple.
/// Si se cumple, muestra un mensaje de error 
/// y throwea un error.
/// </summary>
/// <param name="condition">Condici�n que debe ser falsa.</param>
/// <param name="msg">Mensaje a mostrar, si la condici�n es verdadera.</param>
#define OSK_ASSERT_FALSE(condition, msg) OSK::RuntimeAssertIsFalseFunction(condition, msg, __FUNCTION__, __LINE__, __FILE__)

/// <summary>
/// Comprueba si la condici�n NO se cumple.
/// Si se cumple, muestra un mensaje de aviso.
/// </summary>
/// <param name="condition">Condici�n que debe ser falsa.</param>
/// <param name="msg">Mensaje a mostrar, si la condici�n es verdadera.</param>
#define OSK_CHECK_FALSE(condition, msg) OSK::RuntimeCheckIsFalseFunction(condition, msg, __FUNCTION__, __LINE__, __FILE__)
