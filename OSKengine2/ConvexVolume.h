// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "IBottomLevelCollider.h"

#include "DynamicArray.hpp"

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
		~ConvexVolume() = default;

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
		/// 
		/// @note A la hora de construir el volumen, todas las caras deben tener sus v�rtices
		/// en sentido horiario (si se observa desde fuera del collider).
		void AddFace(const DynamicArray<Vector3f>& vertices);

		/// @brief Optimiza el proceso de detecci�n de colisiones
		/// eliminando ejes duplicados que pueden surgir al registrar
		/// caras paralelas.
		void OptimizeAxes();


		DetailedCollisionInfo GetCollisionInfo(const IBottomLevelCollider& other,
			const ECS::Transform3D& thisOffset, const ECS::Transform3D& otherOffset) const override;

		bool ContainsPoint(const Vector3f& point) const override;

		Vector3f GetFurthestPoint(Vector3f direction) const override;
				

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
		Axis GetLocalSpaceAxis(UIndex32 faceId) const;

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
		Axis GetWorldSpaceAxis(UIndex32 faceId, const ECS::Transform3D& transform) const;

		/// @brief Obtiene una lista con todos los v�rtices del collider transformados
		/// a world-space.
		/// @param transform Transform de este collider.
		/// @return Lista con los v�rtices en world-space.
		DynamicArray<Vector3f> GetWorldSpaceVertices(const ECS::Transform3D& transform) const;

		/// @brief Obtiene los �ndices de todas las caras que contienen el v�rtice dado.
		/// @param vertex V�rtice buscado.
		/// @return Lista con los �ndices de las caras que contienen el v�rtice.
		/// 
		/// @pre El v�rtice debe estar presente en el collider.
		/// 
		/// @note Si se incumple la precondici�n, devuelve una lista vac�a.
		DynamicArray<UIndex32> GetFaceIndicesWithVertex(const Vector3f& vertex) const;


		DynamicArray<FaceIndices> faces;
		DynamicArray<Vector3f> vertices;

	};

}
