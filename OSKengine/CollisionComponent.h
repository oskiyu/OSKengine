#pragma once

#include "ECS.h"

#include "Collider.h"

namespace OSK {

	/// <summary>
	/// Componente ECS que implementa funcionalidad para comprobar colisiones entre las entidades.
	/// </summary>
	class CollisionComponent : public Component {

	public:

		OSK_COMPONENT(CollisionComponent)

		/// <summary>
		/// Colisionador.
		/// </summary>
		Collider& GetCollider() {
			return collider;
		}

	private:

		/// <summary>
		/// Colisionador.
		/// </summary>
		Collider collider;

	};

}