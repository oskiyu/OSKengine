// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "IBottomLevelCollider.h"

#include "DynamicArray.hpp"

namespace OSK::COLLISION {

	class FaceProjection;

	/// @brief Clase que representa un volúmen convexo para la detección de
	/// colisiones detallada.
	class OSKAPI_CALL ConvexVolume final : public IBottomLevelCollider {

		using FaceIndices = DynamicArray<TIndex>;
		using Axis = Vector3f;

	public:

		~ConvexVolume() = default;

		/// @brief Añade una cara al poliedro.
		/// @param vertices Vértices de la cara.
		/// @pre vertices.GetSize() >= 3.
		void AddFace(const DynamicArray<Vector3f>& vertices);

		/// @brief Optimiza el proceso de detección de colisiones
		/// eliminando ejes duplicados que pueden surgir al registrar
		/// caras paralelas.
		void OptimizeAxes();

		bool IsColliding(const IBottomLevelCollider& other,
			const ECS::Transform3D& thisOffset, const ECS::Transform3D& otherOffset) const override;

	private:

		/// @brief Obtiene el eje definido por los puntos de la cara indicada.
		/// Se usa para la optimización de ejes.
		/// 
		/// @param faceId Índice de la cara.
		/// @return Eje de la cara.
		/// 
		/// @note El eje está definido en espacio local.
		/// 
		/// @pre El índice debe apuntar a una cara válida que no haya sido purgada
		/// por el proceso de optimización.
		Axis GetAxisFromFace(TIndex faceId) const;

		/// @brief Obtiene un eje transformado definido por los puntos de la cara indicada.
		/// Se usa para la comprobación de colisiones.
		/// 
		/// @param faceId Índice de la cara.
		/// @param transform Transform aplicado al colisionador.
		/// 
		/// @return Eje de la cara.
		/// 
		/// @pre El índice debe apuntar a una cara válida que no haya sido purgada
		/// por el proceso de optimización.
		Axis GetAxisFromFace(TIndex faceId, const ECS::Transform3D& transform) const;

		DynamicArray<Vector3f> GetTransformedVertices(const ECS::Transform3D& transform) const;

		DynamicArray<FaceIndices> faces;
		DynamicArray<Vector3f> vertices;

	};

}
