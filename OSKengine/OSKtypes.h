#pragma once

#include <cstdint>
#include <cmath>

#include "Color.h"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

/// <summary>
/// Indica el resultado exitoso o no de una operaci�n.
/// </summary>
typedef bool result_t;

/// <summary>
/// Indica el estado de una tecla del teclado. <br/>
/// 'true' = PRESSED<br/>
/// 'false' = RELEASED<br/>
/// </summary>
typedef bool keyState_t;

/// <summary>
/// C�digo que identifica a una tecla del teclado.
/// </summary>
typedef int32_t keyCode_t;

/// <summary>
/// �ndice de un v�rtice.
/// </summary>
typedef uint32_t vertexIndex_t;

/// <summary>
/// Define la precisi�n de las variables de posici�n, �ngulo, zoom, velocidad y sensibilidad de la c�mara.
/// </summary>
typedef double cameraVar_t;

/// <summary>
/// Variaci�n de tiempo (en milisegundos) desde el �ltimo renderizado.
/// Define la precisi�n de la Variaci�n de tiempo.
/// </summary>
typedef float deltaTime_t;

/// <summary>
/// Define la precisi�n de las variables de posici�n y rueda del rat�n.
/// </summary>
typedef float mouseVar_t;

/// <summary>
/// Indica el estado de un bot�n del rat�n. <br/>
/// 'true' = PRESSED <br/>
/// 'false' = RELEASED <br/>
/// </summary>
typedef bool buttonState_t;

/// <summary>
/// C�digo que identifica a un bot�n del rat�n.
/// </summary>
typedef int32_t buttonCode_t;

/// <summary>
/// Cada bit es una flag.
/// </summary>
typedef uint32_t bitFlags_t;
