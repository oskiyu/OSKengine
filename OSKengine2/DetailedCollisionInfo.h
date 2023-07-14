#pragma once

#include "Vector3.hpp"
#include "OSKmacros.h"
#include "DynamicArray.hpp"
#include "GameObject.h"

namespace OSK::COLLISION {
	
	/// @brief Estructura de datos que incluye información adicional
	/// sobre la colisión entre dos entidades.
	class OSKAPI_CALL DetailedCollisionInfo {

	public:

		/// @brief Crea la estructura en el caso de que exista colisión.
		/// @param minimumTranslationVector Vector mínimo que se debe mover la segunda
		/// entidad para que ambas entidades dejen de colisionar.
		/// @param points Puntos de colisión (en espacio del mundo).
		/// @param shouldSwaphObjects True si el objeto de referencia y el incidente están en el orden equivocado.
		/// @return Estructura en el caso de que haya colisión.
		/// 
		/// @remark El vector debe tener dirección @p reference -> @p incident.
		static DetailedCollisionInfo True(
			Vector3f minimumTranslationVector, 
			const DynamicArray<Vector3f>& points,
			bool shouldSwaphObjects);

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
		/// @remark El vector tiene dirección Referencia -> Incidente.
		Vector3f GetMinimumTranslationVector() const;

		/// @brief Devuelve todos los puntos de contacto
		/// entre ambos colliders.
		/// El número de puntos dependerá de cada caso.
		/// @return Lista con los puntos de contacto (en espacio del mundo).
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

		/// @return True si los índices de los objetos deben intercambiarse.
		bool ShouldSwapObjects() const;

	private:

		bool swapObjects = false;

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
