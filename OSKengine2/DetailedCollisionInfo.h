#pragma once

#include "Vector3.hpp"
#include "OSKmacros.h"

namespace OSK::COLLISION {
	
	/// @brief Estructura de datos que incluye informaci�n adicional
	/// sobre la colisi�n entre dos entidades.
	class OSKAPI_CALL DetailedCollisionInfo {

	public:

		/// @brief Crea la estructura en el caso de que exista colisi�n.
		/// @param minimumTranslationVector Vector m�nimo que se debe mover la segunda
		/// entidad para que ambas entidades dejen de colisionar.
		/// @return Estructura en el caso de que haya colisi�n.
		/// 
		/// @remark El vector debe tener direcci�n A -> B.
		static DetailedCollisionInfo True(Vector3f minimumTranslationVector);

		/// @brief Crea una estructura vac�a en el caso de que NO exista colisi�n.
		/// @return Estructura para cuando no hay colisi�n.
		static DetailedCollisionInfo False();


		/// @brief Devuelve true si hay colisi�n entre las dos entidades.
		/// Esto ocurre cuando sus colisionadores de nivel bajo colisionan.
		/// @return True si hay colisi�n entre las dos entidades.
		bool IsColliding() const;

		/// @brief Devuelve el vector m�nimo que se debe mover la segunda
		/// entidad para que ambas entidades dejen de colisionar.
		/// @return Vector m�nimo que se debe mover la segunda
		/// entidad para que ambas entidades dejen de colisionar.
		/// 
		/// @remark El vector tiene direcci�n A -> B.
		Vector3f GetMinimumTranslationVector() const;

	private:

		bool isColliding = false;

		/// @brief 
		/// Este vector representa cu�nto debemos mover los objetos para
		/// que dejen de estar en colisi�n.
		Vector3f minimumTranslationVector = 0.0f;

	};

}
