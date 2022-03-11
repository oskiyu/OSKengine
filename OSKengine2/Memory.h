#pragma once

#include "OSKmacros.h"

namespace OSK::MEMORY {

	/// <summary>
	/// Intercambia dos regiones de memoria.
	/// Las regiones no deben solaparse.
	/// </summary>
	OSKAPI_CALL void MemorySwap(void* a, void* b, TSize numBytes);

}