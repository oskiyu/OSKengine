#pragma once

#include "ApiCall.h"
#include "NumericTypes.h"

namespace OSK::MEMORY {

	/// <summary>
	/// Intercambia dos regiones de memoria.
	/// 
	/// @pre Las regiones no deben solaparse.
	/// </summary>
	OSKAPI_CALL void MemorySwap(void* a, void* b, USize64 numBytes);

}