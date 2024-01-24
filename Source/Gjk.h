#pragma once

#include "Vector3.hpp"
#include "DynamicArray.hpp"

namespace OSK::COLLISION {

	class Simplex;

	/// @brief Soporte de un volúmen 3D.
	/// Indica el punto más alejado en una dirección.
	struct GjkSupport {

		/// @brief Soporte en espacio del mundo.
		Vector3f point = Vector3f::Zero;

		/// @brief Índice del vértice que produjo el soporte.
		UIndex64 originalVertexId = 0;

	};


	/// @brief Soporte de la diferencia de Minkowski.
	struct MinkowskiSupport {

		/// @brief Punto en el hull de la
		/// diferencia de Minkowski.
		Vector3f point = Vector3f::Zero;


		/// @brief Vértice del primer volumen que
		/// produjo el soporte.
		UIndex64 originalVertexIdA = 0;

		/// @brief Vértice del segundo volumen que
		/// produjo el soporte.
		UIndex64 originalVertexIdB = 0;

		bool operator==(const MinkowskiSupport& other) const {
			return originalVertexIdA == other.originalVertexIdA && originalVertexIdB == other.originalVertexIdB;
		}

	};


	/// @brief Interfaz para volúmenes 3D que soporten
	/// la detección de colisiones mediante GJK.
	class OSKAPI_CALL IGjkCollider {

	public:

		virtual ~IGjkCollider() = default;

		/// @param direction Dirección en la que se busca el soporte.
		/// @return Soporte (punto más alejado en la dirección
		/// @p direction).
		virtual GjkSupport GetSupport(const Vector3f& direction) const = 0;

		/// @param direction Dirección en la que se busca el soporte.
		/// @return Soportes (puntos más alejados en la dirección
		/// @p direction).
		virtual DynamicArray<GjkSupport> GetAllSupports(const Vector3f& direction, float epsilon) const = 0;

	};


	/// @param first Primer volúmen 3D.
	/// @param second Segundo volúmen 3D.
	/// @param direction Dirección.
	/// @return Soporte (punto más alejado)
	/// en el hull de la diferencia de Minkowski.
	MinkowskiSupport GetMinkowskiSupport(
		const IGjkCollider& first,
		const IGjkCollider& second,
		const Vector3f& direction);

	/// @param first Primer volúmen 3D.
	/// @param second Segundo volúmen 3D.
	/// @param direction Dirección.
	/// @param epsilon Precisión.
	/// @return Soportes (punto más alejado)
	/// en el hull de la diferencia de Minkowski.
	DynamicArray<MinkowskiSupport> GetAllMinkowskiSupports(
		const IGjkCollider& first,
		const IGjkCollider& second,
		const Vector3f& direction,
		float epsilon);

}
