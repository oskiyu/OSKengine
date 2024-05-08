#pragma once

#include "OSKmacros.h"
#include "HashMap.hpp"

#include "AssetRef.h"
#include "AssetOwningRef.h"

#include "IAsset.h"

#include <shared_mutex>


namespace OSK::ASSETS {

	/// @brief Mapa thread-safe que contiene los assets
	/// de un loader.
	/// @tparam TAssetType Tipo de asset.
	/// 
	/// @threadsafe
	template <typename TAssetType>
	requires std::is_base_of_v<IAsset, TAssetType>
	class AssetsMap {

	public:

		/// @brief Registra un nuevo asset, creando su
		/// AssetOwningRef.
		/// @param assetFilePath Ruta del archivo de descripción que describe el asset.
		/// 
		/// @pre El asset @p assetFilePath no debe haber sido previamente registrado.
		/// 
		/// @threadsafe
		void Register(const std::string& assetFilePath) {
			std::unique_lock lock(m_mutex);
			m_map[static_cast<std::string>(assetFilePath)] = AssetOwningRef<TAssetType>(static_cast<std::string>(assetFilePath));
		}

		/// @param assetFilePath Ruta del archivo de descripción que describe el asset.
		/// @return Referencia al asset indicado.
		/// 
		/// @pre El asset @p assetFilePath debe haber sido previamente registrado.
		/// 
		/// @threadsafe
		AssetRef<TAssetType> GetExistingReference(std::string_view assetFilePath) {
			std::shared_lock lock(m_mutex);

			return m_map.find(assetFilePath)->second.CreateRef();
		}

		/// @brief Registra el asset si no estaba previamente registrado. Después,
		/// devuelve una referencia al asset.
		/// @param assetFilePath Ruta del archivo de descripción que describe el asset.
		/// @return Asset indicado.
		/// 
		/// @pre El asset @p assetFilePath debe haber sido previamente registrado.
		/// 
		/// @threadsafe
		AssetRef<TAssetType> GetReference_OrRegister(std::string_view assetFilePath) {
			{
				std::shared_lock lock(m_mutex);

				if (m_map.contains(assetFilePath)) {
					return m_map.find(assetFilePath)->second.CreateRef();
				}
			}

			std::unique_lock lock(m_mutex);

			m_map[static_cast<std::string>(assetFilePath)] = AssetOwningRef<TAssetType>(static_cast<std::string>(assetFilePath));
			return m_map.find(assetFilePath)->second.CreateRef();
		}


		/// @param assetFilePath Ruta del archivo de descripción del asset.
		/// @return Asset.
		/// 
		/// @pre El asset @p assetFilePath debe haber sido previamente registrado.
		/// 
		/// @stablepointer
		/// @threadsafe
		TAssetType* GetAsset(std::string_view assetFilePath) {
			std::shared_lock lock(m_mutex);
			return m_map.find(assetFilePath)->second.GetAsset();
		}

		/// @param assetFilePath Ruta del archivo de descripción del asset.
		/// @return Si se ha registrado el asset indicado.
		/// 
		/// @threadsafe
		bool HasAsset(std::string_view assetFilePath) const {
			std::shared_lock lock(m_mutex);
			return m_map.contains(assetFilePath);
		}

		/// @brief Establece el flag del asset para indicar que ha sido cargado.
		/// @param assetFilePath Ruta del archivo de descripción que describe el asset.
		/// 
		/// @pre El asset @p assetFilePath debe haber sido previamente registrado.
		/// 
		/// @threadsafe
		void SetAsLoaded(std::string_view assetFilePath) {
			std::shared_lock lock(m_mutex);
			m_map.find(assetFilePath)->second._SetAsLoaded();
		}

	private:

		std::unordered_map<std::string, AssetOwningRef<TAssetType>, StringHasher, std::equal_to<>> m_map;
		mutable std::shared_mutex m_mutex;

	};

}
