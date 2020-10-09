#pragma once

#include "CollisionBox.h"
#include "CollisionSphere.h"
#include "OBB.h"

#include "PhysicalEntityType.h"
#include "CollisionInfo.h"

namespace OSK {

	//Tipos de BroadCollider.
	enum class BroadColliderType {
		BOX_AABB,
		SPHERE
	};

	//Representa un Collider para detección de colisiones 3D.
	class Collider {

	public:

		//Constructor vacío.
		Collider();

		//Crea un collider cuyo BroadCollider será una esfera.
		//	<spherePosition>: posición de la esfera.
		//	<sphereRadius>: radio de la esfera.
		Collider(const Vector3f& spherePosition, const float& sphereRadius);

		//Crea un collider cuyo BroadCollider será una caja AABB.
		//	<boxPosition>: posición de la caja.
		//	<boxSize>: tamaño de la caja.
		Collider(const Vector3f& boxPosition, const Vector3f& boxSize);

		//Establece el BroadCollider, que será una esfera.
		//	<sphere>: esfera que será el broad collider.
		void SetBroadCollider(const CollisionSphere& sphere);

		//Establece el BroadCollider, que será una caja.
		//	<box>: caja que será el broad collider.
		void SetBroadCollider(const CollisionBox& box);

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

		//Devuelve true si este Collider está tocando a otro.
		//	<other>: el otro Collider.
		bool IsColliding(const Collider& other) const;

	private:

		std::vector<OBB> OBBs{};

	};

}
