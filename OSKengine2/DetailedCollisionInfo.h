#pragma once

#include "Vector3.hpp"
#include "OSKmacros.h"
#include "DynamicArray.hpp"
#include "GameObject.h"

namespace OSK::COLLISION {
	
	/// @brief Estructura de datos que incluye informaci�n adicional
	/// sobre la colisi�n entre dos entidades.
	class OSKAPI_CALL DetailedCollisionInfo {

	public:

		/// @brief Crea la estructura en el caso de que exista colisi�n.
		/// @param minimumTranslationVector Vector m�nimo que se debe mover la segunda
		/// entidad para que ambas entidades dejen de colisionar.
		/// @param points Puntos de colisi�n (en espacio del mundo).
		/// @param shouldSwaphObjects True si el objeto de referencia y el incidente est�n en el orden equivocado.
		/// @return Estructura en el caso de que haya colisi�n.
		/// 
		/// @remark El vector debe tener direcci�n @p reference -> @p incident.
		static DetailedCollisionInfo True(
			Vector3f minimumTranslationVector, 
			const DynamicArray<Vector3f>& points,
			bool shouldSwaphObjects);

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
		/// @remark El vector tiene direcci�n Referencia -> Incidente.
		Vector3f GetMinimumTranslationVector() const;

		/// @brief Devuelve todos los puntos de contacto
		/// entre ambos colliders.
		/// El n�mero de puntos depender� de cada caso.
		/// @return Lista con los puntos de contacto (en espacio del mundo).
		/// 
		/// @pre Debe haber una colisi�n entre ambos colliders (IsColliding() debe ser true).
		/// @note Si IsColliding() es true, entonces habr� al menos un punto de contacto.
		/// @note Si IsColliding() es false, entonces devuelve una lista vac�a.
		const DynamicArray<Vector3f>& GetContactPoints() const;

		/// @brief Obtiene un punto de contacto �nico entre ambos colliders.
		/// Si existen varios puntos de contacto, devuelve una media de todos ellos.
		/// @return Punto de contacto.
		/// 
		/// @pre Debe haber colisi�n entre ambos colliders (IsColliding() debe ser true).
		Vector3f GetSingleContactPoint() const;

		/// @return True si los �ndices de los objetos deben intercambiarse.
		bool ShouldSwapObjects() const;

	private:

		bool swapObjects = false;

		bool isColliding = false;

		/// @brief 
		/// Este vector representa cu�nto debemos mover los objetos para
		/// que dejen de estar en colisi�n.
		Vector3f minimumTranslationVector = 0.0f;

		/// @brief Puntos que definen el �rea de contacto.
		DynamicArray<Vector3f> contactPoints;

		Vector3f singleContactPoint = 0.0f;
	};

}
