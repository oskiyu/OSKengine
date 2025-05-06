#pragma once

#include "ApiCall.h"
#include "DefineAs.h"
#include "UniquePtr.hpp"

#include "Gjk.h"
#include "IRayCollider.h"

#include <optional>


namespace OSK::COLLISION {

	class NarrowColliderHolder;

	class OSKAPI_CALL INarrowCollider : public IGjkCollider, public IRayCollider {

	public:

		~INarrowCollider() override = default;

		OSK_DEFINE_AS(INarrowCollider);

		/// @return Copia del collider.
		virtual UniquePtr<INarrowCollider> CreateNarrowCopy() const = 0;

		/// @brief Establece el holder que contiene a
		/// este collider.
		/// @param owner Holder.
		/// 
		/// @note Función interna.
		void _SetNarrowOwner(const NarrowColliderHolder* owner);

		/// @brief Función que se ejecuta cada vez que se transforma
		/// el collider.
		virtual void OnTransform() { (void)0; }

	protected:

		/// @return Holder de alto nivel que contiene
		/// a este collider.
		std::optional<const NarrowColliderHolder*> GetNarrowOwner() const;

	private:

		std::optional<const NarrowColliderHolder*> m_narrowOwner{};

	};

}
