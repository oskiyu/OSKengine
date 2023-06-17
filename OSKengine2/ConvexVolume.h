// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "IBottomLevelCollider.h"

#include "DynamicArray.hpp"

namespace OSK::COLLISION {

	class FaceProjection;

	/// @brief Clase que representa un volúmen convexo para la detección de
	/// colisiones detallada.
	/// 
	/// @note A la hora de construir el volumen, todas las caras deben tener sus vértices
	/// en sentido horiario (si se observa desde fuera del collider).
	class OSKAPI_CALL ConvexVolume final : public IBottomLevelCollider {

	public:

		/// @brief Una lista que contiene los índices de los vértices
		/// de una cara.
		/// 
		/// Cada índice representa un vértice almacenado en la lista de vértices
		/// del collider.
		using FaceIndices = DynamicArray<UIndex32>;

		/// @brief Eje.
		using Axis = Vector3f;

	public:

		ConvexVolume() = default;
		~ConvexVolume() = default;

		/// @brief Crea un volúmen convexo que implementa una caja delimitadora.
		/// @param size Tamaño de la caja, expresado como: { halfWidth, height, halfLenght }.
		/// @param bottomHeight Posición más baja de la caja.
		/// @return Volúmen convexo en modo de caja.
		/// 
		/// La caja final tendrá las dimensiones: { @p size.X / 2, @p size.Y, @p size.Z / 2 }
		/// en la posición { 0, @p bottomHeight, 0 }.
		static ConvexVolume CreateObb(const Vector3f& size, float bottomHeight);


		/// @brief Añade una cara al poliedro.
		/// @param vertices Vértices de la cara.
		/// @pre vertices.GetSize() >= 3.
		/// 
		/// @note A la hora de construir el volumen, todas las caras deben tener sus vértices
		/// en sentido horiario (si se observa desde fuera del collider).
		void AddFace(const DynamicArray<Vector3f>& vertices);

		/// @brief Optimiza el proceso de detección de colisiones
		/// eliminando ejes duplicados que pueden surgir al registrar
		/// caras paralelas.
		void OptimizeAxes();


		DetailedCollisionInfo GetCollisionInfo(const IBottomLevelCollider& other,
			const ECS::Transform3D& thisOffset, const ECS::Transform3D& otherOffset) const override;

		bool ContainsPoint(const Vector3f& point) const override;

		Vector3f GetFurthestPoint(Vector3f direction) const override;
				

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
		Axis GetLocalSpaceAxis(UIndex32 faceId) const;

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
		Axis GetWorldSpaceAxis(UIndex32 faceId, const ECS::Transform3D& transform) const;

		/// @brief Obtiene una lista con todos los vértices del collider transformados
		/// a world-space.
		/// @param transform Transform de este collider.
		/// @return Lista con los vértices en world-space.
		DynamicArray<Vector3f> GetWorldSpaceVertices(const ECS::Transform3D& transform) const;

		/// @brief Obtiene los índices de todas las caras que contienen el vértice dado.
		/// @param vertex Vértice buscado.
		/// @return Lista con los índices de las caras que contienen el vértice.
		/// 
		/// @pre El vértice debe estar presente en el collider.
		/// 
		/// @note Si se incumple la precondición, devuelve una lista vacía.
		DynamicArray<UIndex32> GetFaceIndicesWithVertex(const Vector3f& vertex) const;


		DynamicArray<FaceIndices> faces;
		DynamicArray<Vector3f> vertices;

	};

}
