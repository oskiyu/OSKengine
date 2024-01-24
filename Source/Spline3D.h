#pragma once

#include "OSKmacros.h"
#include "DynamicArray.hpp"
#include "Vector3.hpp"

namespace OSK {

	/// @brief Representa un spline representado por
	/// puntos 3D.
	class OSKAPI_CALL Spline3D {

	public:

		/// @brief A�ade un punto al final del spline.
		/// @param point Nuevo punto.
		void AddPoint(const Vector3f& point);

		/// @return N�mero de puntos del spline.
		USize64 GetPointCount() const;

		/// @brief Obtiene el punto del spline con el �ndice dado.
		/// @param index �ndice (posici�n) del punto.
		/// @return points[index % GetPointCount()]
		const Vector3f& GetPoint(UIndex64 index) const;


		/// @brief Elimina puntos duplicados.
		void Normalize();


		/// @param position Posici�n en el mundo 3D.
		/// @return �ndice del punto del spline m�s cercano.
		UIndex64 GetClosestPointIndex(const Vector3f& position) const;

		/// @param position Posici�n en el mundo 3D.
		/// @param direction Direcci�n de b�squeda.
		/// @return �ndice del punto del spline m�s cercano en la direcci�n dada.
		UIndex64 GetClosestForwardPointIndex(const Vector3f& position, const Vector3f& direction) const;


		/// @param position Posici�n en el mundo 3D.
		/// @return Punto del spline m�s cercano.
		const Vector3f& GetClosestPoint(const Vector3f& position) const;

		/// @param position Posici�n en el mundo 3D.
		/// @param direction Direcci�n de b�squeda.
		/// @return Punto del spline m�s cercano en la direcci�n dada.
		const Vector3f& GetClosestForwardPoint(const Vector3f& position, const Vector3f& direction) const;

	private:

		/// @brief Puntos del spline.
		DynamicArray<Vector3f> m_points;

	};

}
