#pragma once

#include "OSKmacros.h"

#include <string>
#include <memory>

#include "AssetRef.h"

namespace OSK::ASSETS {

	/// @brief Clase que es propietaria de una instancia de un asset.
	/// Permite crear referencias a ese asset.
	/// @tparam TAssetType Clase de asset.
	template <typename TAssetType>
	class AssetOwningRef {

	public:

		AssetOwningRef() = default;
		explicit AssetOwningRef(const std::string& assetPath) : m_data(std::make_unique<TAssetType>(assetPath)) {}

		inline TAssetType* GetAsset() noexcept {
			return m_data.get();
		}

		inline const TAssetType* GetAsset() const noexcept {
			return m_data.get();
		}

		inline TAssetType* operator->() noexcept {
			return m_data.get();
		}

		inline const TAssetType* operator->() const noexcept {
			return m_data.get();
		}

		/// @brief Crea una referencia apuntando a este asset.
		/// @return Nueva referencia.
		inline AssetRef<TAssetType> CreateRef() noexcept {
			return AssetRef<TAssetType>(m_count.get(), m_data.get());
		}

		inline bool IsBeingUsed() const noexcept {
			return *m_count > 0;
		}

	private:

		std::unique_ptr<USize64> m_count = std::make_unique<USize64>(0);
		std::unique_ptr<TAssetType> m_data;

	};

}
