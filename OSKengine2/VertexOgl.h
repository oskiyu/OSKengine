#pragma once

#include "DynamicArray.hpp"

namespace OSK::GRAPHICS {

	struct VertexAttribInfo {

		unsigned int index = 0;
		unsigned int numberOfAttributes = 1;
		unsigned int type;
		unsigned int stride = 0;
		unsigned int offset = 0;

	};

	DynamicArray<VertexAttribInfo> GetAttribInfoOgl_Vertex3D();

}
