#include "Mesh3D.h"

using namespace OSK::GRAPHICS;


Mesh3D::Mesh3D(TSize numIndices, TSize firstIndex) 
	: numIndices(numIndices), firstIndex(firstIndex) {

}

TSize Mesh3D::GetNumberOfIndices() const {
	return numIndices;
}

TSize Mesh3D::GetFirstIndexId() const {
	return firstIndex;
}
