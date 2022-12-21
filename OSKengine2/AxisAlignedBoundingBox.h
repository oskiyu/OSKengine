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
	/// Por defecto, tiene tama�o ("di�metro") 1 
	/// (0.5 a cada lado desde el centro).
	class OSKAPI_CALL AxisAlignedBoundingBox : ITopLevelCollider {

	public:
		
		/// @brief Caja con tama�o 1.
		AxisAlignedBoundingBox() = default;
		/// @param size Tama�o total (la mitad hacia cada direcci�n
		/// desde el centro).
		AxisAlignedBoundingBox(const Vector3f& size);

		/// @param size Tama�o total (la mitad hacia cada direcci�n
		/// desde el centro).
		void SetSize(const Vector3f& size);

		/// @return Tama�o total (la mitad hacia cada direcci�n
		/// desde el centro).
		const Vector3f& GetSize();


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

		Vector3f size = 1.0f;

	};

	using AabbCollider = AxisAlignedBoundingBox;

}
