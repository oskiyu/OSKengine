#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Transform.h"
#include "Collider.h"
#include "PhysicalEntityType.h"

namespace OSK {

	//Representa una entidad presente en una escena de físicas.
	//Su posición y rotación son afectadas por otras entidades y por el motor de físicas.
	struct OSKAPI_CALL PhysicsEntity {

		//Representa si esta entidad se puede mover o no.
		PhysicalEntityMobilityType Type = PhysicalEntityMobilityType::MOVABLE;

		//True si puede moverse en +X.
		bool CanMoveX_p = true;
		//True si puede moverse en -X.
		bool CanMoveX_n = true;

		//True si puede moverse en +Y.
		bool CanMoveY_p = true;
		//True si puede moverse en -Y.
		bool CanMoveY_n = true;

		//True si puede moverse en +Z.
		bool CanMoveZ_p = true;
		//True si puede moverse en -Z.
		bool CanMoveZ_n = true;

		//Puntero al transform de la entidad.
		Transform* EntityTransform = nullptr;

		//Collider de la entidad.
		Collider Collision;

		//Velocidad de la entidad.
		Vector3f Velocity = { 0.0f };
		//Velocidad angular de la entidad.
		Vector3f AngularVelocity = { 0.0f };

		//Masa de la entidad.
		float Mass = 10.0f;

		//Distancia entre el centro de gravedad y el punto más bajo de la entidad.
		float Height = 0.0f;

		//Respuesta ante colisiones de esta entidad.
		bitFlags_t ResponseFlags = PHYSICAL_ENTITY_RESPONSE_MOVE_MTV | PHYSICAL_ENTITY_RESPONSE_ACCEL | PHYSICAL_ENTITY_RESPONSE_ROTATE;

		//Interacción que provoca esta entidad.
		bitFlags_t InteractionFlags = PHYSICAL_ENTITY_INTERACTION_GIVE_ACCEL | PHYSICAL_ENTITY_INTERACTION_GIVE_ROTATION;

		//Obtiene el torque de una fuerza aplicada a la entidad.
		//	<posOfForce>: posición, relativa a la entidad, sobre la que se aplica la fuerza.
		//	<force>: fuerza aplicada.
		inline static Vector3f GetTorque(const Vector3f& posOfForce, const Vector3f& force) {
			return posOfForce.Cross(force);
		}

		inline Vector3f GetPointLinearVelocity(const Vector3f& point) const {
			return Velocity + AngularVelocity.Cross(point);
		}

		bool IsOnFloor = false;

	};

}
