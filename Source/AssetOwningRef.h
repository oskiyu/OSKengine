#pragma once

#include "OSKmacros.h"

#include <string>
#include <memory>
#include "UniquePtr.hpp"

#include "AssetRef.h"

#include "AtomicHolder.h"

namespace OSK::ASSETS {

	/// @brief Clase que es propietaria de una instancia de un asset.
	/// Permite crear referencias a ese asset.
	/// @tparam TAssetType Clase de asset.
	template <typename TAssetType>
	class AssetOwningRef {

	public:

		/// @brief Asset vacío.
		AssetOwningRef() = default;

		/// @brief Crea la referencia que posee el asset.
		/// Crea el asset.
		/// @param assetPath Ruta del asset (archivo de descripción).
		explicit AssetOwningRef(const std::string& assetPath) : m_data(new TAssetType(assetPath)) {}

		~AssetOwningRef() = default;

		OSK_DISABLE_COPY(AssetOwningRef);
		OSK_DEFAULT_MOVE_OPERATOR(AssetOwningRef);


		/// @return True si referencia a algún asset.
		/// Puede que el asset aún no haya sido cargado,
		/// ver HasBeenLoaded().
		/// 
		/// @threadsafety Es thread-safe siempre que se cumplan
		/// las precondiciones.
		bool HasValue() const noexcept {
			return m_data.HasValue();
		}

		/// @return Asset referenciado.
		/// @pre Debe tener un valor (HasValue() == true).
		/// @pre Debe estar cargado (HasBeenLoaded() == true).
		/// 
		/// @threadsafety Es thread-safe siempre que se cumplan
		/// las precondiciones.
		/// @stablepointer
		inline TAssetType* GetAsset() noexcept {
			return m_data.GetPointer();
		}

		/// @return Asset referenciado.
		/// @pre Debe tener un valor (HasValue() == true).
		/// @pre Debe estar cargado (HasBeenLoaded() == true).
		/// 
		/// @threadsafety Es thread-safe siempre que se cumplan
		/// las precondiciones.
		/// @stablepointer
		inline const TAssetType* GetAsset() const noexcept {
			return m_data.GetPointer();
		}

		/// @return Asset referenciado.
		/// @pre Debe tener un valor (HasValue() == true).
		/// @pre Debe estar cargado (HasBeenLoaded() == true).
		/// 
		/// @threadsafety Es thread-safe siempre que se cumplan
		/// las precondiciones.
		/// @stablepointer
		inline TAssetType* operator->() noexcept {
			return m_data.GetPointer();
		}

		/// @return Asset referenciado.
		/// @pre Debe tener un valor (HasValue() == true).
		/// @pre Debe estar cargado (HasBeenLoaded() == true).
		/// 
		/// @threadsafety Es thread-safe siempre que se cumplan
		/// las precondiciones.
		/// @stablepointer
		inline const TAssetType* operator->() const noexcept {
			return m_data.GetPointer();
		}

		/// @brief Crea una referencia apuntando a este asset.
		/// @return Nueva referencia.
		/// 
		/// @pre Debe tener un valor (HasValue() == true).
		/// @pre Debe estar cargado (HasBeenLoaded() == true).
		/// 
		/// @threadsafe
		inline AssetRef<TAssetType> CreateRef() noexcept {
			return AssetRef<TAssetType>(
				m_count.GetPointer(), 
				m_isLoaded.GetPointer(), 
				m_data.GetPointer());
		}

		/// @return True si existe, al menos,
		/// una instancia de AssetRef creada 
		/// por esta referencia.
		/// @threadsafe
		inline bool IsBeingUsed() const noexcept {
			return m_count->load() > 0;
		}

		/// @return Número de AssetRef referenciando
		/// este asset.
		/// @threadsafe
		USize64 GetCount() const {
			return m_count->load();
		}


		/// @return True si ya ha sido cargado.
		/// @threadsafe
		bool HasBeenLoaded() const {
			return m_isLoaded->load();
		}

		/// @brief Establece que el asset ha sido cargado.
		/// @threadsafe
		void _SetAsLoaded() {
			m_isLoaded->store(true);
		}

	private:

		UniquePtr<std::atomic<USize64>> m_count = new std::atomic<USize64>(0);
		UniquePtr<TAssetType> m_data = nullptr;

		UniquePtr<std::atomic<bool>> m_isLoaded = new std::atomic<bool>(false);

	};

}
