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

	/// @brief Clase que representa un vol�men convexo para la detecci�n de
	/// colisiones detallada.
	/// 
	/// @note A la hora de construir el volumen, todas las caras deben tener sus v�rtices
	/// en sentido horiario (si se observa desde fuera del collider).
	class OSKAPI_CALL ConvexVolume final : public IBottomLevelCollider {

	public:

		/// @brief Una lista que contiene los �ndices de los v�rtices
		/// de una cara.
		/// 
		/// Cada �ndice representa un v�rtice almacenado en la lista de v�rtices
		/// del collider.
		using FaceIndices = DynamicArray<UIndex32>;

		/// @brief Eje.
		using Axis = Vector3f;

	public:

		ConvexVolume() = default;
		~ConvexVolume() override = default;

		OwnedPtr<IBottomLevelCollider> CreateCopy() const override;

		/// @brief Crea un vol�men convexo que implementa una caja delimitadora.
		/// @param size Tama�o de la caja, expresado como: { halfWidth, height, halfLenght }.
		/// @param bottomHeight Posici�n m�s baja de la caja.
		/// @return Vol�men convexo en modo de caja.
		/// 
		/// La caja final tendr� las dimensiones: { @p size.X / 2, @p size.Y, @p size.Z / 2 }
		/// en la posici�n { 0, @p bottomHeight, 0 }.
		static ConvexVolume CreateObb(const Vector3f& size, float bottomHeight);


		/// @brief A�ade una cara al poliedro.
		/// @param vertices V�rtices de la cara.
		/// @pre vertices.GetSize() >= 3.
		/// @pre vertices.GetSize() < UINT_MAX.
		/// @pre convexVolume.vertices.GetSize() + vertices.GetSize() < UINT_MAX.
		/// 
		/// @throws InvalidArgumentException si el n�mero de v�rtices proporcionado es incorrecto.
		/// @throws InvalidObjectStateException si el n�mero de v�rtices final es incorrecto.
		void AddFace(const DynamicArray<Vector3f>& vertices);

		/// @brief De-trianguliza el collider, juntando caras paralelas que est�n en el mismo plano.
		void MergeFaces();

		/// @brief Transforma los v�rtices.
		/// @note Debe llamarse una vez por fotograma.
		void Transform(const ECS::Transform3D& transform) override;

		DetailedCollisionInfo GetCollisionInfo(const IBottomLevelCollider& other,
			const ECS::Transform3D& thisOffset, const ECS::Transform3D& otherOffset) const override;

		bool ContainsPoint(const Vector3f& point) const override;

		GjkSupport GetSupport(const Vector3f& direction) const override;
		DynamicArray<GjkSupport> GetAllSupports(const Vector3f& direction, float epsilon) const override;

		/// @brief Devuelve una lista con todos los v�rtices del collider.
		/// Estos v�rtices est�n en espacio local, por lo no se encuentran 
		/// transformados de acuerdo a la entidad que posee el collider.
		/// @return Todos los v�rtices del collider, en espacio local.
		const DynamicArray<Vector3f>& GetLocalSpaceVertices() const;

		const DynamicArray<Vector3f>& GetAxes() const override;
		const DynamicArray<Vector3f>& GetVertices() const override;

		/// @brief Devuelve una lista que contiene una lista de �ndices por cada
		/// cara del collider.
		/// @return Lista con todos los �ndices de cada cara.
		/// 
		/// @see FaceIndices
		DynamicArray<FaceIndices> GetFaceIndices() const;

		UIndex64 GetFaceWithVertexIndices(std::span<const UIndex64, 3> indices, const Vector3f& refNormal) const;

		/// @brief Obtiene un eje transformado definido por los puntos de la cara indicada.
		/// Se usa para la comprobaci�n de colisiones.
		/// 
		/// @param faceId �ndice de la cara.
		/// 
		/// @return Eje de la cara.
		/// 
		/// @pre El �ndice debe apuntar a una cara v�lida que no haya sido purgada
		/// por el proceso de optimizaci�n.
		Axis GetWorldSpaceAxis(UIndex32 faceId) const;

	private:

		UIndex32 GetMostParallelFaceIndex(
			const Vector3f& faceNormal,
			const DynamicArray<UIndex32>& candidates) const;

		UIndex32 GetMostParallelFaceIndex(const Vector3f& faceNormal) const;

		/// @brief Deduplica v�rtices comunes a varias caras.
		void MergeVertices();

		void NormalizeFaces();
		void NormalizeFace(UIndex32 face);

		bool AreFacesEquivalent(UIndex32 firstFace, UIndex32 secondFace) const;

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
