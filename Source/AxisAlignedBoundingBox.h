#pragma once

#include "IBroadCollider.h"
#include "INarrowCollider.h"

#include "Serializer.h"


namespace OSK::COLLISION {

	/// @brief Un �rea de colisi�n de alto nivel representado
	/// por una caja tridimensional.
	/// 
	/// Esta caja est� alineada con los ejes X, Y y Z; y no puede
	/// ser rotada.
	/// 
	/// Por defecto, tiene tama�o ("radio") 1.
	class OSKAPI_CALL AxisAlignedBoundingBox : public IBroadCollider, public INarrowCollider {

	public:

		OSK_SERIALIZABLE();

	public:
		
		/// @brief Caja con tama�o 1.
		explicit AxisAlignedBoundingBox() = default;
		/// @param size Radio (distancia desde un lado hasta el otro).
		explicit AxisAlignedBoundingBox(const Vector3f& size);

		UniquePtr<IBroadCollider>  CreateBroadCopy() const override;
		UniquePtr<INarrowCollider> CreateNarrowCopy() const override;

		GjkSupport GetSupport(const Vector3f& direction) const override;


		/// @param size Radio total (distancia desde un lado hasta el otro).
		void SetSize(const Vector3f& size);

		/// @return Tama�o total (la mitad hacia cada direcci�n
		/// desde el centro).
		const Vector3f& GetSize() const;

		RayCastResult CastRay(const Ray& ray) const override;

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
