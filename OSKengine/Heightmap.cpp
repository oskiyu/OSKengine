#include "Heightmap.h"

using namespace OSK;

Heightmap::~Heightmap() {
	if (data.HasValue())
		delete[] data.GetPointer();
}