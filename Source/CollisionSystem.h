#pragma once

#include "IIteratorSystem.h"

#include "RayCastResult.h"
#include "Ray.h"

#include "BroadCollisionDetectorDispatcher.h"
#include "DetailedCollisionDetectorDispatcher.h"

namespace OSK::COLLISION {
	class Collider;
}

namespace OSK::ECS {

	/// @brief Sistema que se encarga de detectar colisiones entre entidades
	/// y publica los eventos correspondientes.
	/// 
	/// Tipo de evento que publica: `CollisionEvent`.
	class OSKAPI_CALL CollisionSystem final : public IIteratorSystem {

	public:

		OSK_SYSTEM("OSK::CollisionSystem");

		/// @brief También incorpora los detectors
		/// por defecto.
		void OnCreate() override;

		void OnExecutionStart() override;
		void Execute(TDeltaTime deltaTime, std::span<const GameObjectIndex> objects) override;

		COLLISION::RayCastResult CastRay(
			const COLLISION::Ray& ray,
			GameObjectIndex sendingObject) const;

		/// @brief Añade un detector de colisiones.
		/// @param detector Detector a añadir.
		void AddBroadCollisionDetector(UniquePtr<COLLISION::IBroadCollisionDetector>&& detector);

		/// @brief Añade un detector de colisiones.
		/// @param detector Detector a añadir.
		void AddDetailedCollisionDetector(UniquePtr<COLLISION::IDetailedCollisionDetector>&& detector);

	private:

		void ProcessColliderPair(
			GameObjectIndex firstObj,
			GameObjectIndex secondObj,
			const COLLISION::Collider& first,
			const COLLISION::Collider& second) const;

		COLLISION::BroadCollisionDetectorDispatcher    m_broadDispatcher;
		COLLISION::DetailedCollisionDetectorDispatcher m_narrowDispatcher;

	};

}
