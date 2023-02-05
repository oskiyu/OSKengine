#pragma once

#include "Vector3.hpp"
#include "OSKmacros.h"

namespace OSK::COLLISION {
	
	/// @brief Estructura de datos que incluye información adicional
	/// sobre la colisión entre dos entidades.
	class OSKAPI_CALL DetailedCollisionInfo {

	public:

		/// @brief Crea la estructura en el caso de que exista colisión.
		/// @param minimumTranslationVector Vector mínimo que se debe mover la segunda
		/// entidad para que ambas entidades dejen de colisionar.
		/// @return Estructura en el caso de que haya colisión.
		/// 
		/// @remark El vector debe tener dirección A -> B.
		static DetailedCollisionInfo True(Vector3f minimumTranslationVector);

		/// @brief Crea una estructura vacía en el caso de que NO exista colisión.
		/// @return Estructura para cuando no hay colisión.
		static DetailedCollisionInfo False();


		/// @brief Devuelve true si hay colisión entre las dos entidades.
		/// Esto ocurre cuando sus colisionadores de nivel bajo colisionan.
		/// @return True si hay colisión entre las dos entidades.
		bool IsColliding() const;

		/// @brief Devuelve el vector mínimo que se debe mover la segunda
		/// entidad para que ambas entidades dejen de colisionar.
		/// @return Vector mínimo que se debe mover la segunda
		/// entidad para que ambas entidades dejen de colisionar.
		/// 
		/// @remark El vector tiene dirección A -> B.
		Vector3f GetMinimumTranslationVector() const;

	private:

		bool isColliding = false;

		/// @brief 
		/// Este vector representa cuánto debemos mover los objetos para
		/// que dejen de estar en colisión.
		Vector3f minimumTranslationVector = 0.0f;

	};

}
