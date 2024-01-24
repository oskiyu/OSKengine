#pragma once

#include "DynamicArray.hpp"
#include "Vector3.hpp"

namespace OSK::COLLISION {

	/// @param referenceFace Cara de referencia.
	/// @param secondaryFace Cara que será clipped.
	/// @param firstFaceNormal Vector normal de la primera cara.
	/// @param secondFaceNormal Vector normal de la segunda cara.
	/// @return Parche de contacto entre ambas caras.
	DynamicArray<Vector3f> ClipFaces(
		const DynamicArray<Vector3f>& referenceFace,
		const DynamicArray<Vector3f>& secondaryFace,
		const Vector3f& firstFaceNormal,
		const Vector3f& secondFaceNormal);


	/// @return Punto medio de la cara @p face.
	static Vector3f GetCenter(const DynamicArray<Vector3f>& face);

}
