#pragma once

#include "ECS.h"

#include "Collider.h"

namespace OSK {

	/// <summary>
	/// Componente ECS que implementa funcionalidad para comprobar colisiones entre las entidades.
	/// </summary>
	class CollisionComponent : public Component {

	public:

		/// <summary>
		/// Colisionador.
		/// </summary>
		Collider Collider3D;

	};

}