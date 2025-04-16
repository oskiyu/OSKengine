#pragma once

#include "IAsset.h"
#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "UniquePtr.hpp"

#include "Collider.h"

namespace OSK::ASSETS {

	/// @brief Representa un collider cargado a partir de
	/// un archivo externo.
	class OSKAPI_CALL PreBuiltCollider : public IAsset {

	public:

		explicit PreBuiltCollider(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::PreBuiltCollider");

		void _SetCollider(UniquePtr<COLLISION::Collider>&& collider);
		const COLLISION::Collider* GetCollider() const;

	private:

		UniquePtr<COLLISION::Collider> m_collider;

	};

}
