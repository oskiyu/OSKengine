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

	public:

		/// @brief Una lista que contiene los �ndices de los v�rtices
		/// de una cara.
		/// 
		/// Cada �ndice representa un v�rtice almacenado en la lista de v�rtices
		/// del collider.
		using FaceIndices = DynamicArray<TIndex>;

		/// @brief Eje.
		using Axis = Vector3f;

	public:

		ConvexVolume() = default;
		~ConvexVolume() = default;

		/// @brief A�ade una cara al poliedro.
		/// @param vertices V�rtices de la cara.
		/// @pre vertices.GetSize() >= 3.
		void AddFace(const DynamicArray<Vector3f>& vertices);

		/// @brief Optimiza el proceso de detecci�n de colisiones
		/// eliminando ejes duplicados que pueden surgir al registrar
		/// caras paralelas.
		void OptimizeAxes();


		DetailedCollisionInfo GetCollisionInfo(const IBottomLevelCollider& other,
			const ECS::Transform3D& thisOffset, const ECS::Transform3D& otherOffset) const override;

		bool ContainsPoint(const Vector3f& point) const override;
				

		/// @brief Devuelve una lista con todos los v�rtices del collider.
		/// Estos v�rtices est�n en espacio local, por lo no se encuentran 
		/// transformados de acuerdo a la entidad que posee el collider.
		/// @return Todos los v�rtices del collider, en espacio local.
		const DynamicArray<Vector3f>& GetLocalSpaceVertices() const;

		/// @brief Devuelve una lista que contiene una lista de �ndices por cada
		/// cara del collider.
		/// @return Lista con todos los �ndices de cada cara.
		/// 
		/// @see FaceIndices
		const DynamicArray<FaceIndices> GetFaceIndices() const;

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
		Axis GetLocalSpaceAxis(TIndex faceId) const;

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
		Axis GetWorldSpaceAxis(TIndex faceId, const ECS::Transform3D& transform) const;

		/// @brief Obtiene una lista con todos los v�rtices del collider transformados
		/// a world-space.
		/// @param transform Transform de este collider.
		/// @return Lista con los v�rtices en world-space.
		DynamicArray<Vector3f> GetWorldSpaceVertices(const ECS::Transform3D& transform) const;


		DynamicArray<FaceIndices> faces;
		DynamicArray<Vector3f> vertices;

	};

}
