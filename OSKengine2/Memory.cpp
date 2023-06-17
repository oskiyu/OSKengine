#include "Memory.h"

#include <cstring>

void OSK::MEMORY::MemorySwap(void* a, void* b, USize64 numBytes) {
	void* tempBuffer = malloc(numBytes);

	memcpy(tempBuffer, a, numBytes);
	memcpy(a, b, numBytes);
	memcpy(b, tempBuffer, numBytes);

	free(tempBuffer);
}
