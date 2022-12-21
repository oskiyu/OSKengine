// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ITopLevelCollider.h"

namespace OSK::COLLISION {

	/// @brief Un área de colisión de alto nivel representado
	/// por una caja tridimensional.
	/// 
	/// Esta caja está alineada con los ejes X, Y y Z; y no puede
	/// ser rotada.
	/// 
	/// Por defecto, tiene tamaño ("diámetro") 1 
	/// (0.5 a cada lado desde el centro).
	class OSKAPI_CALL AxisAlignedBoundingBox : ITopLevelCollider {

	public:
		
		/// @brief Caja con tamaño 1.
		AxisAlignedBoundingBox() = default;
		/// @param size Tamaño total (la mitad hacia cada dirección
		/// desde el centro).
		AxisAlignedBoundingBox(const Vector3f& size);

		/// @param size Tamaño total (la mitad hacia cada dirección
		/// desde el centro).
		void SetSize(const Vector3f& size);

		/// @return Tamaño total (la mitad hacia cada dirección
		/// desde el centro).
		const Vector3f& GetSize();


		bool IsColliding(const ITopLevelCollider& other,
			const Vector3f& thisOffset, const Vector3f& otherOffset) const override;


		/// @param position Posición del AABB.
		/// @return Esquina mínima: posición del vértice con las
		/// coordenadas más pequeñas.
		Vector3f GetMin(const Vector3f& position) const;

		/// @param position Posición del AABB.
		/// @return Esquina máxima: posición del vértice con las
		/// coordenadas más grandes.
		Vector3f GetMax(const Vector3f& position) const;

	private:

		Vector3f size = 1.0f;

	};

	using AabbCollider = AxisAlignedBoundingBox;

}
