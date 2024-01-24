#pragma once

#include "Vector3.hpp"
#include "DynamicArray.hpp"

#include "FaceProjection.h"

namespace OSK::COLLISION {

	/// @brief Interfaz para vol�menes 3D que soporten
	/// la detecci�n de colisiones mediante SAT.
	class OSKAPI_CALL ISatCollider {

	public:

		virtual ~ISatCollider() = default;

		/// @return Ejes que se deben comprobar.
		virtual const DynamicArray<Vector3f>& GetAxes() const = 0;

		/// @return V�rtices del collider, en espacio del mundo.
		virtual const DynamicArray<Vector3f>& GetVertices() const = 0;

	};


	/// @brief Informaci�n de una comprobaci�n de colisi�n
	/// mediante SAT.
	struct SatInfo {

		/// @brief Informaci�n de una cara.
		struct Face {

			/// @brief Collider al que pertenece la cara.
			enum class Collider {
				A, B
			};

			/// @brief �ndice de la cara.
			UIndex64 index = 0;

			/// @brief Collider al que pertenece la cara.
			Collider collider = Collider::A;

		};


		/// @brief Cara que gener� el eje del MTV.
		Face face{};

		/// @brief True si hay colisi�n, false en caso contrario
		bool overlaps = true;

		/// @brief MTV.
		Vector3f mtv = Vector3f::Zero;

	};


	/// @brief Comprueba si hay una colisi�n entre ambos colliders.
	/// @param first Primer collider.
	/// @param second Segundo collider.
	/// @return Informaci�n de la comprobaci�n.
	SatInfo SatCollision(const ISatCollider& first, const ISatCollider& second);
	
}
