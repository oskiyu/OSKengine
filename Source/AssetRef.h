#pragma once

#include <memory>
#include <atomic>
#include "NumericTypes.h"


namespace OSK::ASSETS {

	/// @brief Referencia a un asset.
	/// @tparam TAssetType Clase de asset.
	template <typename TAssetType>
	class AssetRef {

	public:

		/// @brief Referencia vacía.
		AssetRef() noexcept = default;

		/// @brief Referencia a un asset en concreto.
		/// @param count Puntero al contador de instancias del asset.
		/// @param isLoaded Puntero al flag que indica si ha sido cargado.
		/// @param asset Puntero al propio asset.
		/// 
		/// @pre @p count debe ser estable.
		/// @pre @p asset debe ser estable.
		AssetRef(
			std::atomic<USize64>* count, 
			std::atomic<bool>* isLoaded, 
			TAssetType* asset) noexcept 
			: 
			m_count(count), 
			m_isLoaded(isLoaded), 
			m_asset(asset) 
		{
			m_count->fetch_add(1);
		}

		/// @brief Referencia al mismo asset indicado.
		/// @param other Otro asset.
		AssetRef(const AssetRef& other) noexcept : m_count(other.m_count), m_isLoaded(other.m_isLoaded), m_asset(other.m_asset) {
			m_count->fetch_add(1);
		}

		/// @brief Referencia al mismo asset indicado.
		/// @param other Otro asset.
		/// @return Self.
		AssetRef& operator=(const AssetRef& other) noexcept {
			m_count = other.m_count;
			m_asset = other.m_asset;
			m_isLoaded = other.m_isLoaded;

			m_count->fetch_add(1);

			return *this;
		}

		/// @brief Mueve el otro asset a este.
		/// @param other Otro asset.
		AssetRef(AssetRef&& other) noexcept : m_count(other.m_count), m_isLoaded(other.m_isLoaded), m_asset(other.m_asset) {
			other.m_count = nullptr;
			other.m_asset = nullptr;
			other.m_isLoaded = nullptr;
		}

		/// @brief Mueve el otro asset a este.
		/// @param other Otro asset.
		/// @return Self.
		AssetRef& operator=(AssetRef&& other) noexcept {
			m_count = other.m_count;
			m_asset = other.m_asset;
			m_isLoaded = other.m_isLoaded;
			
			other.m_count = nullptr;
			other.m_asset = nullptr;
			other.m_isLoaded = nullptr;

			return *this;
		}

		~AssetRef() {
			if (m_count) {
				m_count->fetch_sub(1);
			}
		}


		/// @return True si referencia a algún asset.
		/// @threadsafe
		bool HasValue() const noexcept {
			return m_asset;
		}

		/// @return Asset referenciado.
		/// @pre Debe tener valor (HasValue() == true).
		/// 
		/// @threadsafety Es thread-safe siempre que se cumplan
		/// las precondiciones.
		TAssetType* operator->() {
			return m_asset;
		}

		/// @return Asset referenciado.
		/// @pre Debe tener valor (HasValue() == true).
		/// 
		/// @threadsafety Es thread-safe siempre que se cumplan
		/// las precondiciones.
		const TAssetType* operator->() const {
			return m_asset;
		}


		/// @return Asset referenciado.
		/// @pre Debe tener valor (HasValue() == true).
		/// 
		/// @threadsafety Es thread-safe siempre que se cumplan
		/// las precondiciones.
		TAssetType* GetAsset() {
			return m_asset;
		}

		/// @return Asset referenciado.
		/// @pre Debe tener valor (HasValue() == true).
		/// 
		/// @threadsafety Es thread-safe siempre que se cumplan
		/// las precondiciones.
		const TAssetType* GetAsset() const {
			return m_asset;
		}

		/// @return True si ya ha sido cargado.
		/// @threadsafe
		bool HasBeenLoaded() const {
			return m_isLoaded->load();
		}

	private:

		std::atomic<USize64>* m_count = nullptr;
		std::atomic<bool>* m_isLoaded = nullptr;

		TAssetType* m_asset = nullptr;

	};

}
