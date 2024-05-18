// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ITopLevelCollider.h"

#include "Serializer.h"


namespace OSK::COLLISION {

	/// @brief Un �rea de colisi�n de alto nivel representado
	/// por una caja tridimensional.
	/// 
	/// Esta caja est� alineada con los ejes X, Y y Z; y no puede
	/// ser rotada.
	/// 
	/// Por defecto, tiene tama�o ("radio") 1.
	class OSKAPI_CALL AxisAlignedBoundingBox : public ITopLevelCollider {

	public:

		OSK_SERIALIZABLE();

	public:
		
		/// @brief Caja con tama�o 1.
		AxisAlignedBoundingBox() = default;
		/// @param size Radio (distancia desde un lado hasta el otro).
		explicit AxisAlignedBoundingBox(const Vector3f& size);

		OwnedPtr<ITopLevelCollider> CreateCopy() const override;


		/// @param size Radio total (distancia desde un lado hasta el otro).
		void SetSize(const Vector3f& size);

		/// @return Tama�o total (la mitad hacia cada direcci�n
		/// desde el centro).
		const Vector3f& GetSize() const;

		RayCastResult CastRay(const Ray& ray) const override;
		bool ContainsPoint(const Vector3f& point) const override;

		bool IsColliding(const ITopLevelCollider& other) const override;

		bool IsBehindPlane(Plane plane) const override;

		/// @return Esquina m�nima: posici�n del v�rtice con las
		/// coordenadas m�s peque�as.
		Vector3f GetMin() const;

		/// @return Esquina m�xima: posici�n del v�rtice con las
		/// coordenadas m�s grandes.
		Vector3f GetMax() const;

	private:

		Vector3f m_size = Vector3f(1.0f);

	};

	using AabbCollider = AxisAlignedBoundingBox;

}

OSK_SERIALIZATION(OSK::COLLISION::AxisAlignedBoundingBox);
