// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include "IRayCollider.h"
#include "Gjk.h"

#include "UniquePtr.hpp"

namespace OSK::COLLISION {

	class BroadColliderHolder;

	/// @brief Representa un área de colisión no muy ajustada
	/// que permite descartar colisiones de una manera más rápida.
	class OSKAPI_CALL IBroadCollider : public IGjkCollider, public IRayCollider {

	public:

		~IBroadCollider() override = default;

		OSK_DEFINE_AS(IBroadCollider);

		/// @return Copia del collider.
		virtual UniquePtr<IBroadCollider> CreateBroadCopy() const = 0;

		/// @brief Establece el holder que contiene a
		/// este collider.
		/// @param owner Holder.
		/// 
		/// @note Función interna.
		void _SetBroadOwner(const BroadColliderHolder* owner);

	protected:

		/// @return Holder de alto nivel que contiene
		/// a este collider.
		std::optional<const BroadColliderHolder*> GetBroadOwner() const;

	private:

		std::optional<const BroadColliderHolder*> m_broadOwner{};

	};

}
