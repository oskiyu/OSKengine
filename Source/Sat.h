#pragma once

#include "Vector3.hpp"
#include "DynamicArray.hpp"

#include "FaceProjection.h"

namespace OSK::COLLISION {

	/// @brief Interfaz para volúmenes 3D que soporten
	/// la detección de colisiones mediante SAT.
	class OSKAPI_CALL ISatCollider {

	public:

		virtual ~ISatCollider() = default;

		/// @return Ejes que se deben comprobar.
		virtual const DynamicArray<Vector3f>& GetAxes() const = 0;

		/// @return Vértices del collider, en espacio del mundo.
		virtual const DynamicArray<Vector3f>& GetVertices() const = 0;

	};


	/// @brief Información de una comprobación de colisión
	/// mediante SAT.
	struct SatInfo {

		/// @brief Información de una cara.
		struct Face {

			/// @brief Collider al que pertenece la cara.
			enum class Collider {
				A, B
			};

			/// @brief Índice de la cara.
			UIndex64 index = 0;

			/// @brief Collider al que pertenece la cara.
			Collider collider = Collider::A;

		};


		/// @brief Cara que generó el eje del MTV.
		Face face{};

		/// @brief True si hay colisión, false en caso contrario
		bool overlaps = true;

		/// @brief MTV.
		Vector3f mtv = Vector3f::Zero;

	};


	/// @brief Comprueba si hay una colisión entre ambos colliders.
	/// @param first Primer collider.
	/// @param second Segundo collider.
	/// @return Información de la comprobación.
	SatInfo SatCollision(const ISatCollider& first, const ISatCollider& second);
	
}
