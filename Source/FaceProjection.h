// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ApiCall.h"
#include "Vector3.hpp"
#include "DynamicArray.hpp"

namespace OSK::COLLISION {

	/// @brief Proyección de una cara de un poliedro
	/// sobre un eje.
	/// 
	/// El segmento de recta de un eje sobre el que se
	/// encuentra la cara.
	class OSKAPI_CALL FaceProjection {

	public:

		/// @brief Calcula la proyección.
		/// @param vertices Vértices de la cara.
		/// @param axis Eje sobre el que se proyecta.
		FaceProjection(const DynamicArray<Vector3f>& vertices, Vector3f axis);


		/// @return True si ambas proyecciones se solapan.
		/// @pre Las proyecciones deben haberse proyectado sobre el mismo eje.
		bool Overlaps(FaceProjection other) const;

		/// @return Distancia solapada entre ambas proyecciones.
		/// @note Si no se solapan, devuelve 0.
		/// @pre Las proyecciones deben haberse proyectado sobre el mismo eje.
		float GetOverlap(FaceProjection other) const;


		float GetMin() const;
		float GetMax() const;

	private:

		float min = std::numeric_limits<float>::max();
		float max = std::numeric_limits<float>::lowest();

	};

}
