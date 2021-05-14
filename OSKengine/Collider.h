#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "CollisionBox.h"
#include "CollisionSphere.h"
#include "SAT_Collider.h"
#include "SAT_CollisionInfo.h"
#include "ColliderCollisionInfo.h"
#include "BroadColliderUnion.h"

#include "PhysicalEntityType.h"
#include "BroadColliderType.h"

namespace OSK {

	/// <summary>
	/// Representa un Collider para la detección de colisiones 3D.
	/// Contiene un broad collider y varios SAT_Colliders más detallados.
	/// </summary>
	class OSKAPI_CALL Collider {

	public:

		/// <summary>
		/// Crea un collider vacío.
		/// </summary>
		Collider() { }

		/// <summary>
		/// Crea un collider cuyo BroadCollider será una esfera.
		/// </summary>
		/// <param name="spherePosition">Posición de la esfera.</param>
		/// <param name="sphereRadius">Radio de la esfera.</param>
		Collider(const Vector3f& spherePosition, float sphereRadius);

		/// <summary>
		/// Crea un collider cuyo BroadCollider será una caja AABB.
		/// </summary>
		/// <param name="boxPosition">Posición de la caja.</param>
		/// <param name="boxSize">Tamaño de la caja.</param>
		Collider(const Vector3f& boxPosition, const Vector3f& boxSize);

		/// <summary>
		/// Establece el BroadCollider, que será una esfera.
		/// </summary>
		/// <param name="sphere">Esfera que será el broad collider.</param>
		void SetBroadCollider(const CollisionSphere& sphere);

		/// <summary>
		/// Establece el BroadCollider, que será una caja.
		/// </summary>
		/// <param name="box">Caja que será el broad collider.</param>
		void SetBroadCollider(const CollisionBox& box);

		/// <summary>
		/// Establece la posición del BroadCollider, sea del tipo que sea.
		/// </summary>
		/// <param name="pos">Nueva posición.</param>
		void SetPosition(const Vector3f& pos);

		/// <summary>
		/// Devuelve el tipo de BroadCollider de este collider.
		/// </summary>
		/// <returns>Tipo de BroadCollider actual.</returns>
		BroadColliderType GetCurrentBroadColliderType() const;

		/// <summary>
		/// Devuelve el broad collider, que es una collision box.
		/// </summary>
		CollisionBox& GetBroadCollisionBox();

		/// <summary>
		/// Devuelve el broad collider, que es una collision sphere.
		/// </summary>
		CollisionSphere& GetBroadCollisionSphere();

		/// <summary>
		/// Devuelve true si este Collider está tocando a otro.
		/// </summary>
		/// <param name="other">Otro collider.</param>
		/// <returns>True si colisionan.</returns>
		bool IsColliding(Collider& other);

		/// <summary>
		/// Obtiene la información detallada de la colisión entre este Collider y otro.
		/// </summary>
		/// <param name="other">Otro collider.</param>
		/// <returns>Información de la colisión.</returns>
		ColliderCollisionInfo GetCollisionInfo(Collider& other);

		/// <summary>
		/// Transform del collider.
		/// </summary>
		Transform& GetTransform();

		/// <summary>
		/// Añade un nuevo SAT collider a este colider.
		/// </summary>
		void AddCollider(const Collision::SAT_Collider& collider);

		/// <summary>
		/// SAT_Colliders que contiene este collider.
		/// </summary>
		std::vector<Collision::SAT_Collider> satColliders{};

	private:

		/// <summary>
		/// Transform del collider.
		/// </summary>
		Transform transform;

		/// <summary>
		/// BroadCollider.
		/// </summary>
		BroadColliderUnion broadCollider;

		/// <summary>
		/// Tipo de BroadCollider actual.
		/// </summary>
		BroadColliderType broadType = BroadColliderType::BOX_AABB;

	};

}
