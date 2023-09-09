#include "Memory.h"

#include <cstring>
#include <corecrt_malloc.h>

void OSK::MEMORY::MemorySwap(void* a, void* b, USize64 numBytes) {
	void* tempBuffer = malloc(numBytes);

	memcpy(tempBuffer, a, numBytes);
	memcpy(a, b, numBytes);
	memcpy(b, tempBuffer, numBytes);

	free(tempBuffer);
}
