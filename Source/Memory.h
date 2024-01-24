#pragma once

#include "OSKmacros.h"

namespace OSK::MEMORY {

	/// <summary>
	/// Intercambia dos regiones de memoria.
	/// 
	/// @pre Las regiones no deben solaparse.
	/// </summary>
	OSKAPI_CALL void MemorySwap(void* a, void* b, USize64 numBytes);

}