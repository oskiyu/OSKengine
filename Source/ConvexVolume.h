// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "IBottomLevelCollider.h"

#include "DynamicArray.hpp"
#include "Simplex.h"

#include "Gjk.h"
#include "Sat.h"

#include "Serializer.h"


namespace OSK::COLLISION {

	class FaceProjection;

	/// @brief Clase que representa un volúmen convexo para la detección de
	/// colisiones detallada.
	class OSKAPI_CALL ConvexVolume final : public IBottomLevelCollider {

	public:

		template <typename T>
		friend nlohmann::json PERSISTENCE::SerializeJson<T>(const T& data);

		template <typename T>
		friend T PERSISTENCE::DeserializeJson<T>(const nlohmann::json& data);

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

		/// @brief Crea un volumen convexo que implementa una caja delimitadora.
		/// @param size Tamaño de la caja, expresado como radio.
		/// @return Volumen convexo en modo de caja.
		static ConvexVolume CreateObb(const Vector3f& size);

		/// @brief Añade un offset a todos los vértices del volumen.
		/// @param offset Offset.
		/// 
		/// @note No se aplicará a los vértices añadidos después de la llamada.
		void AddOffset(const Vector3f& offset);


		/// @brief Añade una cara al poliedro.
		/// @param vertices Vértices de la cara.
		/// @pre vertices.GetSize() >= 3.
		/// @pre vertices.GetSize() < UINT_MAX.
		/// @pre convexVolume.vertices.GetSize() + vertices.GetSize() < UINT_MAX.
		/// 
		/// @note Los puntos añadidos no serán transformados hasta que no se vuelva
		/// a transformar el collider entero.
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

		RayCastResult CastRay(const Ray& ray) const override;

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


		glm::mat4 m_lastTransform = glm::mat4(1.0f);

		DynamicArray<FaceIndices> m_faces;
		DynamicArray<Vector3f> m_vertices;
		DynamicArray<Vector3f> m_transformedVertices;
		DynamicArray<Vector3f> m_axes;

		Vector3f m_center = Vector3f::Zero;
		Vector3f m_transformedCenter = Vector3f::Zero;

	};

}

namespace OSK::PERSISTENCE {

	template <>
	nlohmann::json SerializeJson<OSK::COLLISION::ConvexVolume>(const OSK::COLLISION::ConvexVolume& data);

	template <>
	OSK::COLLISION::ConvexVolume DeserializeJson<OSK::COLLISION::ConvexVolume>(const nlohmann::json& json);

}
