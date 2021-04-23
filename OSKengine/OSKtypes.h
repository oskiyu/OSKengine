#pragma once

#include <cstdint>
#include <cmath>

#include "Color.h"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

/// <summary>
/// Indica el resultado exitoso o no de una operación.
/// </summary>
typedef bool result_t;

/// <summary>
/// Indica el estado de una tecla del teclado. <br/>
/// 'true' = PRESSED<br/>
/// 'false' = RELEASED<br/>
/// </summary>
typedef bool keyState_t;

/// <summary>
/// Código que identifica a una tecla del teclado.
/// </summary>
typedef int32_t keyCode_t;

/// <summary>
/// Índice de un vértice.
/// </summary>
typedef uint32_t vertexIndex_t;

/// <summary>
/// Define la precisión de las variables de posición, ángulo, zoom, velocidad y sensibilidad de la cámara.
/// </summary>
typedef double cameraVar_t;

/// <summary>
/// Variación de tiempo (en milisegundos) desde el último renderizado.
/// Define la precisión de la Variación de tiempo.
/// </summary>
typedef float deltaTime_t;

/// <summary>
/// Define la precisión de las variables de posición y rueda del ratón.
/// </summary>
typedef float mouseVar_t;

/// <summary>
/// Indica el estado de un botón del ratón. <br/>
/// 'true' = PRESSED <br/>
/// 'false' = RELEASED <br/>
/// </summary>
typedef bool buttonState_t;

/// <summary>
/// Código que identifica a un botón del ratón.
/// </summary>
typedef int32_t buttonCode_t;

/// <summary>
/// Cada bit es una flag.
/// </summary>
typedef uint32_t bitFlags_t;
