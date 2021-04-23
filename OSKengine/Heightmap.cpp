#include "Heightmap.h"

using namespace OSK;

Heightmap::~Heightmap() {
	if (Data != nullptr)
		delete[] Data;
}