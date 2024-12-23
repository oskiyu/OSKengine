#pragma once

#include "IIteratorSystem.h"
#include <optional>


namespace OSK::ECS {

	class TransformComponent3D;

	class OSKAPI_CALL TransformApplierSystem final : public IIteratorSystem {

	public:

		OSK_SYSTEM("OSK::TransformApplierSystem");

		TransformApplierSystem() = default;

		void OnCreate() override;
		void Execute(TDeltaTime deltaTime, std::span<const GameObjectIndex> objects) override;

	private:

		static void Apply(TransformComponent3D& transform, std::optional<const TransformComponent3D*> parent);

	};

}
