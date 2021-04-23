#include "Memory.h"

void* OSK::Memory::AddPtrOffset(const void* originalPointer, size_t offset) {
	const unsigned char* __osk_bytePtr = reinterpret_cast<const unsigned char*>(originalPointer);
	__osk_bytePtr += offset;

	return (void*)reinterpret_cast<const void*>(__osk_bytePtr);
}