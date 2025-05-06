#pragma once

#include "IBroadCollider.h"
#include "INarrowCollider.h"

#include "Serializer.h"


namespace OSK::COLLISION {

	/// @brief Un área de colisión de alto nivel representado
	/// por una caja tridimensional.
	/// 
	/// Esta caja está alineada con los ejes X, Y y Z; y no puede
	/// ser rotada.
	/// 
	/// Por defecto, tiene tamaño ("radio") 1.
	class OSKAPI_CALL AxisAlignedBoundingBox : public IBroadCollider, public INarrowCollider {

	public:

		OSK_SERIALIZABLE();

	public:
		
		/// @brief Caja con tamaño 1.
		explicit AxisAlignedBoundingBox() = default;
		/// @param size Radio (distancia desde un lado hasta el otro).
		explicit AxisAlignedBoundingBox(const Vector3f& size);

		UniquePtr<IBroadCollider>  CreateBroadCopy() const override;
		UniquePtr<INarrowCollider> CreateNarrowCopy() const override;

		GjkSupport GetSupport(const Vector3f& direction) const override;


		/// @param size Radio total (distancia desde un lado hasta el otro).
		void SetSize(const Vector3f& size);

		/// @return Tamaño total (la mitad hacia cada dirección
		/// desde el centro).
		const Vector3f& GetSize() const;

		RayCastResult CastRay(const Ray& ray) const override;

		/// @return Esquina mínima: posición del vértice con las
		/// coordenadas más pequeñas.
		Vector3f GetMin() const;

		/// @return Esquina máxima: posición del vértice con las
		/// coordenadas más grandes.
		Vector3f GetMax() const;

	private:

		Vector3f m_size = Vector3f(1.0f);

	};

	using AabbCollider = AxisAlignedBoundingBox;

}

OSK_SERIALIZATION(OSK::COLLISION::AxisAlignedBoundingBox);
