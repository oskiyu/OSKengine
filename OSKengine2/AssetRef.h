#pragma once

#include "OSKmacros.h"

#include <memory>

namespace OSK::ASSETS {

	/// @brief Referencia a un asset.
	/// @tparam TAssetType Clase de asset.
	template <typename TAssetType>
	class AssetRef {

	public:

		AssetRef() noexcept = default;
		explicit AssetRef(USize64* count, TAssetType* asset) noexcept : m_count(count), m_asset(asset) {
			(*m_count)++;
		}

		AssetRef(const AssetRef& other) noexcept : m_count(other.m_count), m_asset(other.m_asset) {
			(*m_count)++;
		}

		AssetRef& operator=(const AssetRef& other) noexcept {
			m_count = other.m_count;
			m_asset = other.m_asset;
			(*m_count)++;
			return *this;
		}

		AssetRef(AssetRef&& other) noexcept : m_count(other.m_count), m_asset(other.m_asset) {
			other.m_count = nullptr;
			other.m_asset = nullptr;
		}

		AssetRef& operator=(AssetRef&& other) noexcept {
			m_count = other.m_count;
			m_asset = other.m_asset;
			
			other.m_count = nullptr;
			other.m_asset = nullptr;

			return *this;
		}

		~AssetRef() {
			if (m_count) {
				(*m_count)--;
			}
		}


		bool HasValue() const noexcept {
			return m_asset;
		}

		TAssetType* operator->() {
			return m_asset;
		}

		const TAssetType* operator->() const {
			return m_asset;
		}

		
		TAssetType* GetAsset() {
			return m_asset;
		}

		const TAssetType* GetAsset() const {
			return m_asset;
		}

	private:

		USize64* m_count = nullptr;
		TAssetType* m_asset = nullptr;

	};

}
