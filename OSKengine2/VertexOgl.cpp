#include "VertexOgl.h"

#include "Vertex.h"
#include "OglTypes.h"

OSK::DynamicArray<OSK::VertexAttribInfo> OSK::GetAttribInfoOgl_Vertex3D() {
	DynamicArray<VertexAttribInfo> output;

	VertexAttribInfo info{};
	info.index = 0;
	info.numberOfAttributes = 3;
	info.type = GL_FLOAT;
	info.offset = offsetof(Vertex3D, position);
	info.stride = sizeof(Vertex3D);

	output.Insert(info);

	info.index = 1;
	info.numberOfAttributes = 4;
	info.type = GL_FLOAT;
	info.offset = offsetof(Vertex3D, color);
	info.stride = sizeof(Vertex3D);

	output.Insert(info);

	info.index = 2;
	info.numberOfAttributes = 4;
	info.type = GL_FLOAT;
	info.offset = offsetof(Vertex3D, texCoords);
	info.stride = sizeof(Vertex3D);

	output.Insert(info);

	return output;
}
