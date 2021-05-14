#include "Heightmap.h"

using namespace OSK;

Heightmap::~Heightmap() {
	if (data != nullptr)
		delete[] data;
}