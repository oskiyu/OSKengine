#pragma once

#include "CollisionBox.h"
#include "CollisionSphere.h"
#include "SAT_Collider.h"
#include "SAT_CollisionInfo.h"

#include "PhysicalEntityType.h"
#include "CollisionInfo.h"

namespace OSK {


	struct ColliderCollisionInfo {
		bool IsColliding = false;
		bool IsBroadColliderColliding = false;
		Collision::SAT_Collider* SAT_1 = nullptr;
		Collision::SAT_Collider* SAT_2 = nullptr;
	};


	//Tipos de BroadCollider.
	enum class BroadColliderType {
		BOX_AABB,
		SPHERE
	};

	//Representa un Collider para detecci�n de colisiones 3D.
	class Collider {

	public:

		//Constructor vac�o.
		Collider();

		//Crea un collider cuyo BroadCollider ser� una esfera.
		//	<spherePosition>: posici�n de la esfera.
		//	<sphereRadius>: radio de la esfera.
		Collider(const Vector3f& spherePosition, const float& sphereRadius);

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

		void SetPosition(const Vector3f& pos);

		//BroadCollider.
		union __BroadCollider_t {
			CollisionBox Box;
			CollisionSphere Sphere;

			inline __BroadCollider_t() {
				memset(this, 0, sizeof(*this));
			}
			//BroadCollider.
		} BroadCollider;

		//Tipo de BroadCollider actual.
		BroadColliderType type = BroadColliderType::BOX_AABB;

		//Devuelve true si este Collider est� tocando a otro.
		//	<other>: el otro Collider.
		bool IsColliding(Collider& other);

		ColliderCollisionInfo GetCollisionInfo(Collider& other);

		Transform transform;

		std::vector<Collision::SAT_Collider> OBBs{};

	};

}
