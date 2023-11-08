// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "IBottomLevelCollider.h"

#include "DynamicArray.hpp"
#include "Simplex.h"

#include "Gjk.h"
#include "Sat.h"

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
		~ConvexVolume() override = default;

		OwnedPtr<IBottomLevelCollider> CreateCopy() const override;

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
		/// @pre vertices.GetSize() < UINT_MAX.
		/// @pre convexVolume.vertices.GetSize() + vertices.GetSize() < UINT_MAX.
		/// 
		/// @throws InvalidArgumentException si el número de vértices proporcionado es incorrecto.
		/// @throws InvalidObjectStateException si el número de vértices final es incorrecto.
		void AddFace(const DynamicArray<Vector3f>& vertices);

		/// @brief De-trianguliza el collider, juntando caras paralelas que están en el mismo plano.
		void MergeFaces();

		/// @brief Transforma los vértices.
		/// @note Debe llamarse una vez por fotograma.
		void Transform(const ECS::Transform3D& transform) override;

		DetailedCollisionInfo GetCollisionInfo(const IBottomLevelCollider& other,
			const ECS::Transform3D& thisOffset, const ECS::Transform3D& otherOffset) const override;

		bool ContainsPoint(const Vector3f& point) const override;

		GjkSupport GetSupport(const Vector3f& direction) const override;
		DynamicArray<GjkSupport> GetAllSupports(const Vector3f& direction, float epsilon) const override;

		/// @brief Devuelve una lista con todos los vértices del collider.
		/// Estos vértices están en espacio local, por lo no se encuentran 
		/// transformados de acuerdo a la entidad que posee el collider.
		/// @return Todos los vértices del collider, en espacio local.
		const DynamicArray<Vector3f>& GetLocalSpaceVertices() const;

		const DynamicArray<Vector3f>& GetAxes() const override;
		const DynamicArray<Vector3f>& GetVertices() const override;

		/// @brief Devuelve una lista que contiene una lista de índices por cada
		/// cara del collider.
		/// @return Lista con todos los índices de cada cara.
		/// 
		/// @see FaceIndices
		DynamicArray<FaceIndices> GetFaceIndices() const;

		UIndex64 GetFaceWithVertexIndices(std::span<const UIndex64, 3> indices, const Vector3f& refNormal) const;

		/// @brief Obtiene un eje transformado definido por los puntos de la cara indicada.
		/// Se usa para la comprobación de colisiones.
		/// 
		/// @param faceId Índice de la cara.
		/// 
		/// @return Eje de la cara.
		/// 
		/// @pre El índice debe apuntar a una cara válida que no haya sido purgada
		/// por el proceso de optimización.
		Axis GetWorldSpaceAxis(UIndex32 faceId) const;

	private:

		UIndex32 GetMostParallelFaceIndex(
			const Vector3f& faceNormal,
			const DynamicArray<UIndex32>& candidates) const;

		UIndex32 GetMostParallelFaceIndex(const Vector3f& faceNormal) const;

		/// @brief Deduplica vértices comunes a varias caras.
		void MergeVertices();

		void NormalizeFaces();
		void NormalizeFace(UIndex32 face);

		bool AreFacesEquivalent(UIndex32 firstFace, UIndex32 secondFace) const;

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




		DynamicArray<UIndex32> GetFaceIndicesWithVertex(UIndex32 vertexIndex) const;

		void RecalculateCenter();


		DynamicArray<FaceIndices> m_faces;
		DynamicArray<Vector3f> m_vertices;
		DynamicArray<Vector3f> m_transformedVertices;
		DynamicArray<Vector3f> m_axes;

		Vector3f m_center = Vector3f::Zero;
		Vector3f m_transformedCenter = Vector3f::Zero;

	};

}
