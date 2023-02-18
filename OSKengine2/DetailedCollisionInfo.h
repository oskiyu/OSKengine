#pragma once

#include "Vector3.hpp"
#include "OSKmacros.h"
#include "DynamicArray.hpp"

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
		static DetailedCollisionInfo True(Vector3f minimumTranslationVector, const DynamicArray<Vector3f>& points);

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

		/// @brief Devuelve todos los puntos de contacto
		/// entre ambos colliders.
		/// El número de puntos dependerá de cada caso.
		/// @return Lista con los puntos de contacto.
		/// 
		/// @pre Debe haber una colisión entre ambos colliders (IsColliding() debe ser true).
		/// @note Si IsColliding() es true, entonces habrá al menos un punto de contacto.
		/// @note Si IsColliding() es false, entonces devuelve una lista vacía.
		const DynamicArray<Vector3f>& GetContactPoints() const;

		/// @brief Obtiene un punto de contacto único entre ambos colliders.
		/// Si existen varios puntos de contacto, devuelve una media de todos ellos.
		/// @return Punto de contacto.
		/// 
		/// @pre Debe haber colisión entre ambos colliders (IsColliding() debe ser true).
		Vector3f GetSingleContactPoint() const;

	private:

		bool isColliding = false;

		/// @brief 
		/// Este vector representa cuánto debemos mover los objetos para
		/// que dejen de estar en colisión.
		Vector3f minimumTranslationVector = 0.0f;

		/// @brief Puntos que definen el área de contacto.
		DynamicArray<Vector3f> contactPoints;

		Vector3f singleContactPoint = 0.0f;
	};

}
