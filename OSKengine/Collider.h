#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "CollisionBox.h"
#include "CollisionSphere.h"
#include "SAT_Collider.h"
#include "SAT_CollisionInfo.h"

#include "PhysicalEntityType.h"

namespace OSK {


	//Estructura que contiene informaci�n de una colisi�n entre Colliders.
	struct OSKAPI_CALL ColliderCollisionInfo {
		//True si hay colisi�n.
		bool IsColliding = false;
		//True si hay colisi�n entre los BroadColliders.
		//No tiene por qu� suponer una colisi�n real.
		bool IsBroadColliderColliding = false;
		//Primer sat que colisiona.
		Collision::SAT_Collider* SAT_1 = nullptr;
		//Segundo sat que colisiona.
		Collision::SAT_Collider* SAT_2 = nullptr;
	};


	//Tipos de BroadCollider.
	enum class OSKAPI_CALL BroadColliderType {
		//AABB.
		BOX_AABB,
		//Esfera.
		SPHERE
	};

	//Representa un Collider para la detecci�n de colisiones 3D.
	class OSKAPI_CALL Collider {

	public:

		//Constructor vac�o.
		Collider();

		//Crea un collider cuyo BroadCollider ser� una esfera.
		//	<spherePosition>: posici�n de la esfera.
		//	<sphereRadius>: radio de la esfera.
		Collider(const Vector3f& spherePosition, float sphereRadius);

		//Crea un collider cuyo BroadCollider ser� una caja AABB.
		//	<boxPosition>: posici�n de la caja.
		//	<boxSize>: tama�o de la caja.
		Collider(const Vector3f& boxPosition, const Vector3f& boxSize);

		//Establece el BroadCollider, que ser� una esfera.
		//	<sphere>: esfera que ser� el broad collider.
		void SetBroadCollider(const CollisionSphere& sphere);

		//Establece el BroadCollider, que ser� una caja.
		//	<box>: caja que ser� el broad collider.
		void SetBroadCollider(const CollisionBox& box);

		//Establece la posici�n del BroadCollider, sea del tipo que sea.
		//	<pos>: posici�n.
		void SetPosition(const Vector3f& pos);

		//BroadCollider.
		union OSKAPI_CALL __BroadCollider_t {
			//AABB.
			CollisionBox Box;
			//Esfera.
			CollisionSphere Sphere;

			//Inicia el BroadCollider (vac�o).
			inline __BroadCollider_t() {
				memset(this, 0, sizeof(*this));
			}
			//BroadCollider.
		} BroadCollider;

		//Tipo de BroadCollider actual.
		BroadColliderType BroadType = BroadColliderType::BOX_AABB;

		//Devuelve true si este Collider est� tocando a otro.
		//	<other>: el otro Collider.
		bool IsColliding(Collider& other);

		//Obtiene la informaci�n de la colisi�n entre este Collider y otro.
		//	<other>: otro collider.
		ColliderCollisionInfo GetCollisionInfo(Collider& other);

		//Transform del collider.
		Transform ColliderTransform;

		//SAT_Colliders que contiene este collider.
		std::vector<Collision::SAT_Collider> SatColliders{};

	};

}
