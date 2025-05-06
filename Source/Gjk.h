#pragma once

#include "Vector3.hpp"
#include "DynamicArray.hpp"

#include "Plane.h"
#include "Frustum.h"

#include <optional>

namespace OSK::COLLISION {

	class Simplex;

	/// @brief Soporte de un vol�men 3D.
	/// Indica el punto m�s alejado en una direcci�n.
	struct GjkSupport {

		/// @brief Soporte en espacio del mundo.
		Vector3f point = Vector3f::Zero;

		/// @brief �ndice del v�rtice que produjo el soporte.
		/// S�lamente para colisionadores con v�rtices.
		std::optional<UIndex64> originalVertexId;

	};


	/// @brief Soporte de la diferencia de Minkowski.
	struct MinkowskiSupport {

		/// @brief Punto en el hull de la
		/// diferencia de Minkowski.
		Vector3f point = Vector3f::Zero;


		/// @brief �ndice del v�rtice del primer volumen que
		/// produjo el soporte.
		std::optional<UIndex64> originalVertexIdA;

		/// @brief �ndice del v�rtice del segundo volumen que
		/// produjo el soporte.
		std::optional<UIndex64> originalVertexIdB;
		
		/// V�rtice del primer volumen que
		/// produjo el soporte.
		std::optional<Vector3f> worldSpacePointA;

		/// V�rtice del segundo volumen que
		/// produjo el soporte.
		std::optional<Vector3f> worldSpacePointB;

		bool operator==(const MinkowskiSupport& other) const {
			return originalVertexIdA == other.originalVertexIdA && originalVertexIdB == other.originalVertexIdB;
		}

	};


	/// @brief Interfaz para vol�menes 3D que soporten
	/// la detecci�n de colisiones mediante GJK.
	/// 
	/// Clase base para todos los colisionadores del sistema.
	class OSKAPI_CALL IGjkCollider {

	public:

		virtual ~IGjkCollider() = default;

		/// @param direction Direcci�n en la que se busca el soporte.
		/// @return Soporte (punto m�s alejado en la direcci�n
		/// @p direction).
		virtual GjkSupport GetSupport(const Vector3f& direction) const = 0;

		/// @brief Comrpueba si un punto est� dentro del collider.
		/// @param point Punto a comprobar.
		/// @return True si el punto est� dentro del collider.
		virtual bool ContainsPoint(const Vector3f& point) const;


		/// @brief Comprueba si parte del collider est� por detr�s del plano.
		/// @param plane Plano a comprobar.
		/// @return True si est� completamente por detr�s.
		bool IsBehindPlane(Plane plane) const;

		/// @brief Comprueba si el collider est� COMPLETMENTE por detr�s del plano.
		/// @param plane Plano a comprobar.
		/// @return True si est� completamente por detr�s.
		bool IsFullyBehindPlane(Plane plane) const;

		/// @return True si el collider est� dentro del frustum indicado.
		bool IsInsideFrustum(const AnyFrustum& frustum) const;

	};


	/// @param first Primer vol�men 3D.
	/// @param second Segundo vol�men 3D.
	/// @param direction Direcci�n.
	/// @return Soporte (punto m�s alejado)
	/// en el hull de la diferencia de Minkowski.
	MinkowskiSupport GetMinkowskiSupport(
		const IGjkCollider& first,
		const IGjkCollider& second,
		const Vector3f& direction);

}
