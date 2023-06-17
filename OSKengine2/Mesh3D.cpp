#include "Mesh3D.h"

using namespace OSK::GRAPHICS;


Mesh3D::Mesh3D(USize32 numIndices, UIndex32 firstIndex)
	: numIndices(numIndices), firstIndex(firstIndex) {

}

USize32 Mesh3D::GetNumberOfIndices() const {
	return numIndices;
}

UIndex32 Mesh3D::GetFirstIndexId() const {
	return firstIndex;
}
