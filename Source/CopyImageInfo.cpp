#include "CopyImageInfo.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


CopyImageInfo CopyImageInfo::CreateDefault1D(uint32_t size) {
	CopyImageInfo output{};
	output.SetCopySize(size);

	return output;
}

CopyImageInfo CopyImageInfo::CreateDefault2D(Vector2ui size) {
	CopyImageInfo output{};
	output.SetCopySize(size);

	return output;
}

CopyImageInfo CopyImageInfo::CreateDefault3D(Vector3ui size) {
	CopyImageInfo output{};
	output.SetCopySize(size);

	return output;
}

void CopyImageInfo::SetCopySize(uint32_t size) {
	copySize = { size, 1u, 1u };
}

void CopyImageInfo::SetCopySize(Vector2ui size) {
	copySize = { size.x, size.y, 1u };
}

void CopyImageInfo::SetCopySize(Vector3ui size) {
	copySize = size;
}

void CopyImageInfo::SetSourceOffset(uint32_t offset) {
	sourceOffset = { offset, 0, 0 };
}

void CopyImageInfo::SetSourceOffset(Vector2ui offset) {
	sourceOffset = { offset.x, offset.y, 0 };
}

void CopyImageInfo::SetSourceOffset(Vector3ui offset) {
	sourceOffset = offset;
}

void CopyImageInfo::SetDestinationOffset(uint32_t offset) {
	destinationOffset = { offset, 0, 0 };
}

void CopyImageInfo::SetDestinationOffset(Vector2ui offset) {
	destinationOffset = { offset.x, offset.y, 0 };
}

void CopyImageInfo::SetDestinationOffset(Vector3ui offset) {
	destinationOffset = offset;
}

void CopyImageInfo::SetCopyAllLevels() {
	numArrayLevels = ALL_ARRAY_LEVELS;
}
