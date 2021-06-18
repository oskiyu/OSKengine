#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Transform.h"
#include "Collider.h"
#include "PhysicalEntityType.h"
#include "Component.h"

namespace OSK {

	/// <summary>
	/// Componente que representa una entidad presente en una escena de físicas.
	/// Su posición y rotación son afectadas por otras entidades y por el motor de físicas.
	/// </summary>
	struct PhysicsComponent : public Component {

		friend class PhysicsSystem;

	public:

		OSK_COMPONENT(PhysicsComponent)

		PhysicalEntityMobilityType GetMobility() const {
			return mobilityType;
		}

		void SetMobility(PhysicalEntityMobilityType mobility) {
			mobilityType = mobility;
		}

		Collider& GetCollider() {
			return collider;
		}

		/// <summary>
		/// Velocidad de la entidad.
		/// </summary>
		Vector3f velocity = { 0.0f };

		/// <summary>
		/// Velocidad angular de la entidad.
		/// </summary>
		Vector3f angularVelocity = { 0.0f };

		/// <summary>
		/// Masa de la entidad.
		/// </summary>
		float mass = 10.0f;

		/// <summary>
		/// Distancia entre el centro de gravedad y el punto más bajo de la entidad.
		/// </summary>
		float gravityCenterHeight = 0.0f;

		/// <summary>
		/// True si la entidad está apoyada sobre el terreno.
		/// </summary>
		bool IsOnFloor() const {
			return isOnFloor;
		}

	private:

		/// <summary>
		/// Representa si esta entidad se puede mover o no.
		/// </summary>
		PhysicalEntityMobilityType mobilityType = PhysicalEntityMobilityType::MOVABLE;

		/// <summary>
		/// True si puede moverse en +X.
		/// </summary>
		bool canMoveX_p = true;

		/// <summary>
		/// True si puede moverse en -X.
		/// </summary>
		bool canMoveX_n = true;

		/// <summary>
		/// True si puede moverse en +Y.
		/// </summary>
		bool canMoveY_p = true;

		/// <summary>
		/// True si puede moverse en -Y.
		/// </summary>
		bool canMoveY_n = true;

		/// <summary>
		/// True si puede moverse en +Z.
		/// </summary>
		bool canMoveZ_p = true;

		/// <summary>
		/// True si puede moverse en -Z.
		/// </summary>
		bool canMoveZ_n = true;

		/// <summary>
		/// Collider de la entidad.
		/// </summary>
		Collider collider;
		
		/// <summary>
		/// Respuesta ante colisiones de esta entidad.
		/// </summary>
		bitFlags_t responseFlags = PHYSICAL_ENTITY_RESPONSE_MOVE_MTV | PHYSICAL_ENTITY_RESPONSE_ACCEL | PHYSICAL_ENTITY_RESPONSE_ROTATE;

		/// <summary>
		/// Interacción que provoca esta entidad.
		/// </summary>
		bitFlags_t interactionFlags = PHYSICAL_ENTITY_INTERACTION_GIVE_ACCEL | PHYSICAL_ENTITY_INTERACTION_GIVE_ROTATION;

		/// <summary>
		/// Obtiene el torque de una fuerza aplicada a la entidad.
		/// </summary>
		/// <param name="posOfForce">Posición, relativa a la entidad, sobre la que se aplica la fuerza</param>
		/// <param name="force">Fuerza aplicada.</param>
		/// <returns>Torque.</returns>
		inline static Vector3f GetTorque(const Vector3f& posOfForce, const Vector3f& force) {
			return posOfForce.Cross(force);
		}

		/// <summary>
		/// Obtiene la velocidad lineal de un punto de la entidad.
		/// </summary>
		/// <param name="point">Punto de la entidad.</param>
		/// <returns>Velocidad lineal del punto.</returns>
		inline Vector3f GetPointLinearVelocity(const Vector3f& point) const {
			return velocity + angularVelocity.Cross(point);
		}

		/// <summary>
		/// True si la entidad está apoyada sobre el terreno.
		/// </summary>
		bool isOnFloor = false;

	};

}
