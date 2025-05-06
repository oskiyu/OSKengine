#pragma once

#include "Vector3.hpp"
#include "DynamicArray.hpp"

#include "Plane.h"
#include "Frustum.h"

#include <optional>

namespace OSK::COLLISION {

	class Simplex;

	/// @brief Soporte de un volúmen 3D.
	/// Indica el punto más alejado en una dirección.
	struct GjkSupport {

		/// @brief Soporte en espacio del mundo.
		Vector3f point = Vector3f::Zero;

		/// @brief Índice del vértice que produjo el soporte.
		/// Sólamente para colisionadores con vértices.
		std::optional<UIndex64> originalVertexId;

	};


	/// @brief Soporte de la diferencia de Minkowski.
	struct MinkowskiSupport {

		/// @brief Punto en el hull de la
		/// diferencia de Minkowski.
		Vector3f point = Vector3f::Zero;


		/// @brief Índice del vértice del primer volumen que
		/// produjo el soporte.
		std::optional<UIndex64> originalVertexIdA;

		/// @brief Índice del vértice del segundo volumen que
		/// produjo el soporte.
		std::optional<UIndex64> originalVertexIdB;
		
		/// Vértice del primer volumen que
		/// produjo el soporte.
		std::optional<Vector3f> worldSpacePointA;

		/// Vértice del segundo volumen que
		/// produjo el soporte.
		std::optional<Vector3f> worldSpacePointB;

		bool operator==(const MinkowskiSupport& other) const {
			return originalVertexIdA == other.originalVertexIdA && originalVertexIdB == other.originalVertexIdB;
		}

	};


	/// @brief Interfaz para volúmenes 3D que soporten
	/// la detección de colisiones mediante GJK.
	/// 
	/// Clase base para todos los colisionadores del sistema.
	class OSKAPI_CALL IGjkCollider {

	public:

		virtual ~IGjkCollider() = default;

		/// @param direction Dirección en la que se busca el soporte.
		/// @return Soporte (punto más alejado en la dirección
		/// @p direction).
		virtual GjkSupport GetSupport(const Vector3f& direction) const = 0;

		/// @brief Comrpueba si un punto está dentro del collider.
		/// @param point Punto a comprobar.
		/// @return True si el punto está dentro del collider.
		virtual bool ContainsPoint(const Vector3f& point) const;


		/// @brief Comprueba si parte del collider está por detrás del plano.
		/// @param plane Plano a comprobar.
		/// @return True si está completamente por detrás.
		bool IsBehindPlane(Plane plane) const;

		/// @brief Comprueba si el collider está COMPLETMENTE por detrás del plano.
		/// @param plane Plano a comprobar.
		/// @return True si está completamente por detrás.
		bool IsFullyBehindPlane(Plane plane) const;

		/// @return True si el collider está dentro del frustum indicado.
		bool IsInsideFrustum(const AnyFrustum& frustum) const;

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

}
