// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ApiCall.h"k
#include "DetailedCollisionInfo.h"
#include "GameObject.h"

#include "DynamicArray.hpp"

namespace OSK::COLLISION {

	/// @brief Información sobre el resultado de la detección
	/// de colisiones entre dos colliders.
	class OSKAPI_CALL CollisionInfo {

	public:

		/// @return Collision info para el caso en el que
		/// no hay colisión.
		static CollisionInfo False();

		/// @return Collision info para el caso en el que
		/// no hay colisión, pero los colliders de alto nivel
		/// sí que colisionan.
		static CollisionInfo TopLevelOnly();

		/// @brief Collision info para el caso en el que
		/// sí que hay colisión.
		/// @param info Información detallada de la colisión.
		/// @return Collision info para el caso en el que
		/// sí que hay colisión.
		static CollisionInfo True(const DynamicArray<DetailedCollisionInfo>& info);


		/// @brief Devuelve true si hay una colisión entre las entidades,
		/// esto es, si existe una colisión entre los colliders de bajo
		/// nivel.
		/// 
		/// @return True si hay colisión entre las dos entidades.
		/// 
		/// @remark Si devuelve true, entoncces IsTopLevelColliding también es true.
		/// @remark Si devuelve false, IsTopLevelColliding puede ser true o false.
		bool IsColliding() const;

		/// @brief Devuelve true si hay una colisión entre los colliders de alto
		/// nivel de las dos entidades.
		/// Que haya colisión entre los colliders de alto nivel no implica que
		/// haya una colisión entre las entidades, habrá que comprobarlo con IsColliding.
		/// 
		/// @return True si hay una colisión entre los colliders de alto
		/// nivel de las dos entidades.
		/// 
		/// @remark Si es true, entonces IsColliding será true o false, dependiendo de cada caso.
		/// @remark Si es false, IsColliding es falso también.
		bool IsTopLevelColliding() const;


		/// @brief Devvuelve información detallada sobre la colisión,
		/// en caso de que haya.
		/// @return Información detallada.
		/// 
		/// @pre IsColliding debe devolver true. De lo contrario
		/// la información devuelta es inválida.
		const DynamicArray<DetailedCollisionInfo>& GetDetailedInfo() const;
		
	private:

		CollisionInfo(bool isColliding, bool isTopLevelColliding);

		bool isColliding = false;
		bool isTopLevelColliding = false;

		DynamicArray<DetailedCollisionInfo> detailedInfo;

	};

}
