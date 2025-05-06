#pragma once

#include "IBroadCollider.h"
#include "UniquePtr.hpp"
#include "Vector3.hpp"

#include <type_traits>

namespace OSK::COLLISION {


	/// @brief Contiene un collider de alto nivel.
	class OSKAPI_CALL BroadColliderHolder final {
	public:

		explicit BroadColliderHolder(UniquePtr<IBroadCollider>&& collider);

		/// @return Copia de este collider.
		UniquePtr<BroadColliderHolder> CreateCopy() const;

		IBroadCollider* GetCollider();
		const IBroadCollider* GetCollider() const;

		/// @return Posición en la que se encuentra
		/// el collider.
		const Vector3f& GetPosition() const;

		/// @brief Establece la posición
		/// del collider.
		/// @param position Nueva posición del collider.
		void UpdatePosition(const Vector3f& position);

	private:

		UniquePtr<IBroadCollider> m_collider{};
		Vector3f m_position = Vector3f::Zero;

	};

	/// @tparam T Tipo del collider de alto nivel.
	/// @param collider Collider de alto nivel.
	/// @return Collider como alto nivel.
	template <typename T>
		requires std::is_base_of_v<IBroadCollider, T>
	UniquePtr<BroadColliderHolder> AsBroad(UniquePtr<T>&& collider) {
		return MakeUnique<BroadColliderHolder>(std::move(collider));
	}

}
