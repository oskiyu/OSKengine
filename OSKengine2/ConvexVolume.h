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

	public:

		/// @brief Una lista que contiene los índices de los vértices
		/// de una cara.
		/// 
		/// Cada índice representa un vértice almacenado en la lista de vértices
		/// del collider.
		using FaceIndices = DynamicArray<TIndex>;

		/// @brief Eje.
		using Axis = Vector3f;

	public:

		ConvexVolume() = default;
		~ConvexVolume() = default;

		/// @brief Añade una cara al poliedro.
		/// @param vertices Vértices de la cara.
		/// @pre vertices.GetSize() >= 3.
		void AddFace(const DynamicArray<Vector3f>& vertices);

		/// @brief Optimiza el proceso de detección de colisiones
		/// eliminando ejes duplicados que pueden surgir al registrar
		/// caras paralelas.
		void OptimizeAxes();


		DetailedCollisionInfo GetCollisionInfo(const IBottomLevelCollider& other,
			const ECS::Transform3D& thisOffset, const ECS::Transform3D& otherOffset) const override;

		bool ContainsPoint(const Vector3f& point) const override;
				

		/// @brief Devuelve una lista con todos los vértices del collider.
		/// Estos vértices están en espacio local, por lo no se encuentran 
		/// transformados de acuerdo a la entidad que posee el collider.
		/// @return Todos los vértices del collider, en espacio local.
		const DynamicArray<Vector3f>& GetLocalSpaceVertices() const;

		/// @brief Devuelve una lista que contiene una lista de índices por cada
		/// cara del collider.
		/// @return Lista con todos los índices de cada cara.
		/// 
		/// @see FaceIndices
		const DynamicArray<FaceIndices> GetFaceIndices() const;

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
		Axis GetLocalSpaceAxis(TIndex faceId) const;

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
		Axis GetWorldSpaceAxis(TIndex faceId, const ECS::Transform3D& transform) const;

		/// @brief Obtiene una lista con todos los vértices del collider transformados
		/// a world-space.
		/// @param transform Transform de este collider.
		/// @return Lista con los vértices en world-space.
		DynamicArray<Vector3f> GetWorldSpaceVertices(const ECS::Transform3D& transform) const;


		DynamicArray<FaceIndices> faces;
		DynamicArray<Vector3f> vertices;

	};

}
