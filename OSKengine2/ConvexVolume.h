// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "IBottomLevelCollider.h"

#include "DynamicArray.hpp"

namespace OSK::COLLISION {

	class FaceProjection;

	/// @brief Clase que representa un vol�men convexo para la detecci�n de
	/// colisiones detallada.
	class OSKAPI_CALL ConvexVolume final : public IBottomLevelCollider {

		using FaceIndices = DynamicArray<TIndex>;
		using Axis = Vector3f;

	public:

		~ConvexVolume() = default;

		/// @brief A�ade una cara al poliedro.
		/// @param vertices V�rtices de la cara.
		/// @pre vertices.GetSize() >= 3.
		void AddFace(const DynamicArray<Vector3f>& vertices);

		/// @brief Optimiza el proceso de detecci�n de colisiones
		/// eliminando ejes duplicados que pueden surgir al registrar
		/// caras paralelas.
		void OptimizeAxes();

		bool IsColliding(const IBottomLevelCollider& other,
			const ECS::Transform3D& thisOffset, const ECS::Transform3D& otherOffset) const override;

	private:

		/// @brief Obtiene el eje definido por los puntos de la cara indicada.
		/// Se usa para la optimizaci�n de ejes.
		/// 
		/// @param faceId �ndice de la cara.
		/// @return Eje de la cara.
		/// 
		/// @note El eje est� definido en espacio local.
		/// 
		/// @pre El �ndice debe apuntar a una cara v�lida que no haya sido purgada
		/// por el proceso de optimizaci�n.
		Axis GetAxisFromFace(TIndex faceId) const;

		/// @brief Obtiene un eje transformado definido por los puntos de la cara indicada.
		/// Se usa para la comprobaci�n de colisiones.
		/// 
		/// @param faceId �ndice de la cara.
		/// @param transform Transform aplicado al colisionador.
		/// 
		/// @return Eje de la cara.
		/// 
		/// @pre El �ndice debe apuntar a una cara v�lida que no haya sido purgada
		/// por el proceso de optimizaci�n.
		Axis GetAxisFromFace(TIndex faceId, const ECS::Transform3D& transform) const;

		DynamicArray<Vector3f> GetTransformedVertices(const ECS::Transform3D& transform) const;

		DynamicArray<FaceIndices> faces;
		DynamicArray<Vector3f> vertices;

	};

}
