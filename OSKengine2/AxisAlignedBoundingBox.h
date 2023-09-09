// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ITopLevelCollider.h"

namespace OSK::COLLISION {

	/// @brief Un �rea de colisi�n de alto nivel representado
	/// por una caja tridimensional.
	/// 
	/// Esta caja est� alineada con los ejes X, Y y Z; y no puede
	/// ser rotada.
	/// 
	/// Por defecto, tiene tama�o ("radio") 0.5.
	class OSKAPI_CALL AxisAlignedBoundingBox : public ITopLevelCollider {

	public:
		
		/// @brief Caja con tama�o 1.
		AxisAlignedBoundingBox() = default;
		/// @param size Radio (distancia desde un lado hasta el otro).
		explicit AxisAlignedBoundingBox(const Vector3f& size);

		/// @param size Radio total (distancia desde un lado hasta el otro).
		void SetSize(const Vector3f& size);

		/// @return Tama�o total (la mitad hacia cada direcci�n
		/// desde el centro).
		const Vector3f& GetSize() const;

		/// @todo Implementaci�n. 
		RayCastResult CastRay(const Ray& ray, const Vector3f& position) const override;
		bool ContainsPoint(const Vector3f& thisOffset, const Vector3f& point) const override;
		bool IsColliding(const ITopLevelCollider& other,
			const Vector3f& thisOffset, const Vector3f& otherOffset) const override;


		/// @param position Posici�n del AABB.
		/// @return Esquina m�nima: posici�n del v�rtice con las
		/// coordenadas m�s peque�as.
		Vector3f GetMin(const Vector3f& position) const;

		/// @param position Posici�n del AABB.
		/// @return Esquina m�xima: posici�n del v�rtice con las
		/// coordenadas m�s grandes.
		Vector3f GetMax(const Vector3f& position) const;

	private:

		Vector3f m_size = 1.0f;

	};

	using AabbCollider = AxisAlignedBoundingBox;

}
