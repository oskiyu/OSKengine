#pragma once

#include "Vector3.hpp"
#include "DynamicArray.hpp"

namespace OSK::COLLISION {

	class Simplex;

	/// @brief Soporte de un vol�men 3D.
	/// Indica el punto m�s alejado en una direcci�n.
	struct GjkSupport {

		/// @brief Soporte en espacio del mundo.
		Vector3f point = Vector3f::Zero;

		/// @brief �ndice del v�rtice que produjo el soporte.
		UIndex64 originalVertexId = 0;

	};


	/// @brief Soporte de la diferencia de Minkowski.
	struct MinkowskiSupport {

		/// @brief Punto en el hull de la
		/// diferencia de Minkowski.
		Vector3f point = Vector3f::Zero;


		/// @brief V�rtice del primer volumen que
		/// produjo el soporte.
		UIndex64 originalVertexIdA = 0;

		/// @brief V�rtice del segundo volumen que
		/// produjo el soporte.
		UIndex64 originalVertexIdB = 0;

		bool operator==(const MinkowskiSupport& other) const {
			return originalVertexIdA == other.originalVertexIdA && originalVertexIdB == other.originalVertexIdB;
		}

	};


	/// @brief Interfaz para vol�menes 3D que soporten
	/// la detecci�n de colisiones mediante GJK.
	class OSKAPI_CALL IGjkCollider {

	public:

		virtual ~IGjkCollider() = default;

		/// @param direction Direcci�n en la que se busca el soporte.
		/// @return Soporte (punto m�s alejado en la direcci�n
		/// @p direction).
		virtual GjkSupport GetSupport(const Vector3f& direction) const = 0;

		/// @param direction Direcci�n en la que se busca el soporte.
		/// @return Soportes (puntos m�s alejados en la direcci�n
		/// @p direction).
		virtual DynamicArray<GjkSupport> GetAllSupports(const Vector3f& direction, float epsilon) const = 0;

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

	/// @param first Primer vol�men 3D.
	/// @param second Segundo vol�men 3D.
	/// @param direction Direcci�n.
	/// @param epsilon Precisi�n.
	/// @return Soportes (punto m�s alejado)
	/// en el hull de la diferencia de Minkowski.
	DynamicArray<MinkowskiSupport> GetAllMinkowskiSupports(
		const IGjkCollider& first,
		const IGjkCollider& second,
		const Vector3f& direction,
		float epsilon);

}
