#include "VertexInfo.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

TIndex VertexInfo::GetSize() const {
	TIndex size = 0;

	for (const auto& entry : entries)
		size += entry.size;

	return size;
}
