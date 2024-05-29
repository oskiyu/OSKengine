// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ApiCall.h"k
#include "DetailedCollisionInfo.h"
#include "GameObject.h"

#include "DynamicArray.hpp"

namespace OSK::COLLISION {

	/// @brief Informaci�n sobre el resultado de la detecci�n
	/// de colisiones entre dos colliders.
	class OSKAPI_CALL CollisionInfo {

	public:

		/// @return Collision info para el caso en el que
		/// no hay colisi�n.
		static CollisionInfo False();

		/// @return Collision info para el caso en el que
		/// no hay colisi�n, pero los colliders de alto nivel
		/// s� que colisionan.
		static CollisionInfo TopLevelOnly();

		/// @brief Collision info para el caso en el que
		/// s� que hay colisi�n.
		/// @param info Informaci�n detallada de la colisi�n.
		/// @return Collision info para el caso en el que
		/// s� que hay colisi�n.
		static CollisionInfo True(const DynamicArray<DetailedCollisionInfo>& info);


		/// @brief Devuelve true si hay una colisi�n entre las entidades,
		/// esto es, si existe una colisi�n entre los colliders de bajo
		/// nivel.
		/// 
		/// @return True si hay colisi�n entre las dos entidades.
		/// 
		/// @remark Si devuelve true, entoncces IsTopLevelColliding tambi�n es true.
		/// @remark Si devuelve false, IsTopLevelColliding puede ser true o false.
		bool IsColliding() const;

		/// @brief Devuelve true si hay una colisi�n entre los colliders de alto
		/// nivel de las dos entidades.
		/// Que haya colisi�n entre los colliders de alto nivel no implica que
		/// haya una colisi�n entre las entidades, habr� que comprobarlo con IsColliding.
		/// 
		/// @return True si hay una colisi�n entre los colliders de alto
		/// nivel de las dos entidades.
		/// 
		/// @remark Si es true, entonces IsColliding ser� true o false, dependiendo de cada caso.
		/// @remark Si es false, IsColliding es falso tambi�n.
		bool IsTopLevelColliding() const;


		/// @brief Devvuelve informaci�n detallada sobre la colisi�n,
		/// en caso de que haya.
		/// @return Informaci�n detallada.
		/// 
		/// @pre IsColliding debe devolver true. De lo contrario
		/// la informaci�n devuelta es inv�lida.
		const DynamicArray<DetailedCollisionInfo>& GetDetailedInfo() const;
		
	private:

		CollisionInfo(bool isColliding, bool isTopLevelColliding);

		bool isColliding = false;
		bool isTopLevelColliding = false;

		DynamicArray<DetailedCollisionInfo> detailedInfo;

	};

}
