#pragma once

#include <cstdint>
#include <cmath>

#include "Color.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

//[float_t]
//Precisión normal.
typedef float_t oskFloat_t;


//[double]
//Precisión alta.
typedef double oskFloatPrecise_t;


//[bool]
//Indica el resultado exitoso o no de una operación.
typedef bool result_t;


//[bool]
//Indica el estado de una tecla del teclado.
// 'true' = PRESSED
// 'false' = RELEASED
typedef bool keyState_t;


//[int32_t]
//Código que identifica a una tecla del teclado.
typedef int32_t keyCode_t;


//[uint32_t]
//Índice de un vértice.
typedef uint32_t vertexIndex_t;


//[uint32_t]
//ID de un VertexArrayObject (VAO)
typedef uint32_t vertexArrayObject_t;


//[uint32_t]
//ID de un BufferObject (<>BO)
typedef uint32_t bufferObject_t;


//[oskFloat_t]
//Define la precisión de las variables de posición, ángulo, zoom, velocidad y sensibilidad de la cámara.
// <oskFloat_t>: precisión normal (float_t)
// <oskFloatPrecise_t>: precisión alta (double)
typedef oskFloat_t cameraVar_t;


//[oskFloat_t]
//Variación de tiempo (en milisegundos) desde el último renderizado.
//Define la precisión de la Variación de tiempo.
// <oskFloat_t>: precisión normal (float_t)
// <oskFloatPrecise_t>: precisión alta (double)
typedef oskFloat_t deltaTime_t;


//[oskFloat_t]
//Define la precisión de las variables de posición y rueda del ratón.
// <oskFloat_t>: precisión normal (float_t)
// <oskFloatPrecise_t>: precisión alta (double)
typedef oskFloat_t mouseVar_t;


//[bool]
//Indica el estado de un botón del ratón.
// 'true' = PRESSED
// 'false' = RELEASED
typedef bool buttonState_t;


//[int32_t]
//Código que identifica a un botón del ratón.
typedef int32_t buttonCode_t;


//[uint32_t]
//ID de una textura de OpenGL.
//NO USAR: usar OSK::Texture u OSK::Skybox.
typedef uint32_t textureCode_t;


typedef uint32_t bitFlags_t;