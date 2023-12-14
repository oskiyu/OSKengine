#pragma once

#include "OSKmacros.h"
#include "DynamicArray.hpp"
#include "Vector3.hpp"

namespace OSK {

	/// @brief Representa un spline representado por
	/// puntos 3D.
	class OSKAPI_CALL Spline3D {

	public:

		/// @brief Añade un punto al final del spline.
		/// @param point Nuevo punto.
		void AddPoint(const Vector3f& point);

		/// @return Número de puntos del spline.
		USize64 GetPointCount() const;

		/// @brief Obtiene el punto del spline con el índice dado.
		/// @param index Índice (posición) del punto.
		/// @return points[index % GetPointCount()]
		const Vector3f& GetPoint(UIndex64 index) const;


		/// @brief Elimina puntos duplicados.
		void Normalize();


		/// @param position Posición en el mundo 3D.
		/// @return Índice del punto del spline más cercano.
		UIndex64 GetClosestPointIndex(const Vector3f& position) const;

		/// @param position Posición en el mundo 3D.
		/// @param direction Dirección de búsqueda.
		/// @return Índice del punto del spline más cercano en la dirección dada.
		UIndex64 GetClosestForwardPointIndex(const Vector3f& position, const Vector3f& direction) const;


		/// @param position Posición en el mundo 3D.
		/// @return Punto del spline más cercano.
		const Vector3f& GetClosestPoint(const Vector3f& position) const;

		/// @param position Posición en el mundo 3D.
		/// @param direction Dirección de búsqueda.
		/// @return Punto del spline más cercano en la dirección dada.
		const Vector3f& GetClosestForwardPoint(const Vector3f& position, const Vector3f& direction) const;

	private:

		/// @brief Puntos del spline.
		DynamicArray<Vector3f> m_points;

	};

}
