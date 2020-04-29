#pragma once

#include <cstdint>
#include <cmath>

#include "Color.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

//[float_t]
//Precisi�n normal.
typedef float_t oskFloat_t;


//[double]
//Precisi�n alta.
typedef double oskFloatPrecise_t;


//[bool]
//Indica el resultado exitoso o no de una operaci�n.
typedef bool result_t;


//[bool]
//Indica el estado de una tecla del teclado.
// 'true' = PRESSED
// 'false' = RELEASED
typedef bool keyState_t;


//[int32_t]
//C�digo que identifica a una tecla del teclado.
typedef int32_t keyCode_t;


//[uint32_t]
//�ndice de un v�rtice.
typedef uint32_t vertexIndex_t;


//[uint32_t]
//ID de un VertexArrayObject (VAO)
typedef uint32_t vertexArrayObject_t;


//[uint32_t]
//ID de un BufferObject (<>BO)
typedef uint32_t bufferObject_t;


//[oskFloat_t]
//Define la precisi�n de las variables de posici�n, �ngulo, zoom, velocidad y sensibilidad de la c�mara.
// <oskFloat_t>: precisi�n normal (float_t)
// <oskFloatPrecise_t>: precisi�n alta (double)
typedef oskFloat_t cameraVar_t;


//[oskFloat_t]
//Variaci�n de tiempo (en milisegundos) desde el �ltimo renderizado.
//Define la precisi�n de la Variaci�n de tiempo.
// <oskFloat_t>: precisi�n normal (float_t)
// <oskFloatPrecise_t>: precisi�n alta (double)
typedef oskFloat_t deltaTime_t;


//[oskFloat_t]
//Define la precisi�n de las variables de posici�n y rueda del rat�n.
// <oskFloat_t>: precisi�n normal (float_t)
// <oskFloatPrecise_t>: precisi�n alta (double)
typedef oskFloat_t mouseVar_t;


//[bool]
//Indica el estado de un bot�n del rat�n.
// 'true' = PRESSED
// 'false' = RELEASED
typedef bool buttonState_t;


//[int32_t]
//C�digo que identifica a un bot�n del rat�n.
typedef int32_t buttonCode_t;


//[uint32_t]
//ID de una textura de OpenGL.
//NO USAR: usar OSK::Texture u OSK::Skybox.
typedef uint32_t textureCode_t;


typedef uint32_t bitFlags_t;