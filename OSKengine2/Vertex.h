#pragma once

#include <string>
#include "VertexInfo.h"

namespace OSK::GRAPHICS {
	using TIndexSize = uint32_t;
}

#define OSK_VERTEX_TYPE_REG(name) const static std::string GetVertexTypeName() { return name; } \
const static VertexInfo GetVertexInfo();
