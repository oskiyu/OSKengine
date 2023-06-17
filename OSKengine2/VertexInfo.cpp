#include "VertexInfo.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

USize32 VertexInfo::GetSize() const {
	USize32 size = 0;

	for (const auto& entry : entries)
		size += entry.size;

	return size;
}
