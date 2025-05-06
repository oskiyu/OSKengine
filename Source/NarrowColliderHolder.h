#pragma once

#include "INarrowCollider.h"
#include "UniquePtr.hpp"
#include "Vector3.hpp"
#include "Transform3D.h"

#include <type_traits>

namespace OSK::COLLISION {


	/// @brief Contiene un collider de alto nivel.
	class OSKAPI_CALL NarrowColliderHolder final {

	public:
		
		explicit NarrowColliderHolder(UniquePtr<INarrowCollider>&& collider);

		/// @return Copia de este collider.
		UniquePtr<NarrowColliderHolder> CreateCopy() const;

		INarrowCollider* GetCollider();
		const INarrowCollider* GetCollider() const;

		/// @brief Transforma el collider.
		/// Debe llamarse una vez por frame.
		/// @param transform Transform de la entidad.
		void Transform(const Transform3D& transform);

		/// @return Transform del collider.
		const Transform3D& GetTransform() const;
	private:

		UniquePtr<INarrowCollider> m_collider{};
		Transform3D m_transform{};

	};


	/// @tparam T Tipo del collider de alto nivel.
	/// @param collider Collider de alto nivel.
	/// @return Collider como alto nivel.
	template <typename T>
	requires std::is_base_of_v<INarrowCollider, T>
	UniquePtr<NarrowColliderHolder> AsNarrow(UniquePtr<T>&& collider) {
		return MakeUnique<NarrowColliderHolder>(std::move(collider));
	}

}
