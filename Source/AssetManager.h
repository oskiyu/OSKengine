#pragma once

#include <string>

#include "HashMap.hpp"
#include "SharedPtr.hpp"
#include "UniquePtr.hpp"
#include "IAssetLoader.h"
#include "IAsset.h"
#include "Logger.h"
#include "AssetRef.h"

#include "AssetLoaderNotFoundException.h"
#include "Assert.h"

#include <span>


namespace OSK::ASSETS {

	/// @brief
	/// El AssetManager se encarga de manejar los loaders de los assets y los lifetimes
	/// de los assets.
	/// 
	/// @note Se deben registrar todos los loaders mediante RegisterLoader().
	/// 
	/// @note Al cargar un asset mediante Load(), debemos especificar a qué lifetime corresponde.
	/// Un lifetime agrupa assets que tienen un ciclo de vida común. Al terminar el ciclo de
	/// vida, todos los assets del lifetime son eliminados.
	/// 
	/// @note Esta clase es dueña de los assets.
	/// 
	/// @note Usa un sistema de cacheado, por lo que los assets se cargan una única vez.
	/// 
	/// @warning El nombre de los assets cargados debe ser único, independientemente del loader
	/// con el que haya sido cargado.
	class AssetManager {

	public:

		/// @brief Carga un asset.
		/// @tparam T Tipo del asset.
		/// @param assetFilePath Ruta del archivo de descripción del asset.
		/// @return Referencia al asset.
		/// 
		/// @pre Debe haberse registrado un loader para el tipo de asset.
		/// 
		/// @throws AssetLoaderNotFoundException si el cargador para el tipo
		/// de asset no ha sido previamente regsitrado.
		template <typename T> 
		AssetRef<T> Load(const std::string& assetFilePath) {
			OSK_ASSERT(m_loaders.contains(T::GetAssetType()), AssetLoaderNotFoundException(T::GetAssetType()));

			AssetRef<T> output = AssetRef<T>();

			m_loaders.at(T::GetAssetType())->FullyLoad(assetFilePath, &output);

			return output;
		}

		/// @brief Carga un asset de manera asíncrona.
		/// Lanza un trabajo que realizará la carga del asset.
		/// @tparam T Tipo de asset.
		/// @param assetFilePath Ruta del archivo de descripción.
		/// @return Referencia al asset.
		/// 
		/// @note La referencia no contendrá el asset cargado al instante debido a
		/// la ejecución asíncrona. Debe comprobarse antes de usarse.
		/// 
		/// @pre Debe haberse registrado un loader para el tipo de asset.
		/// 
		/// @throws AssetLoaderNotFoundException si el cargador para el tipo
		/// de asset no ha sido previamente regsitrado.
		template <typename T>
		AssetRef<T> LoadAsync(const std::string& assetFilePath, std::span<const std::string> tags = {}) {
			OSK_ASSERT(m_loaders.contains(T::GetAssetType()), AssetLoaderNotFoundException(T::GetAssetType()))

			AssetRef<T> output = AssetRef<T>();

			auto* loader = m_loaders.find(T::GetAssetType())->second.GetPointer();

			loader->RegisterWithoutLoading(assetFilePath, std::addressof(output));
			LaunchAsyncLoad(assetFilePath, loader, tags);
			
			return output;
		}


		/// @brief Registra un loader, para poder cargar assets de un tipo determinado.
		/// @tparam T Tipo del loader.
		/// 
		/// @pre No debe haberse registrado antes este loader.
		/// @throws AssetLoaderAlreadyRegisteredException si se incumple la precondición.
		template <typename T> 
		void RegisterLoader() {
			const auto& key = T::GetAssetType();

			OSK_ASSERT(!m_loaders.contains(key), AssetLoaderAlreadyRegisteredException(key))
			m_loaders[T::GetAssetType()] = new T;
		}

		/// @brief Devuelve el loader indicado.
		/// @tparam TLoader Loader a obtener.
		/// @return Loader.
		/// 
		/// @pre El loader debe haber sido previamente registrado.
		/// @throws AssetLoaderNotFoundException si el loader no ha sido previamente cargado.
		template <typename TLoader>
		TLoader* GetLoader() {
			const auto& key = TLoader::GetAssetType();

			OSK_ASSERT(m_loaders.contains(TLoader::GetAssetType()), AssetLoaderNotFoundException(TLoader::GetAssetType()))

			return static_cast<TLoader*>(m_loaders.find(key)->second.GetPointer());
		}

	private:

		/// @brief Lanza un trabajo para cargar el asset indicado.
		/// @param assetPath Ruta al archivo de descripción.
		/// @param loader Cargador que debe realizar la carga.
		/// 
		/// @pre Debe haberse registrado un loader para el tipo de asset.
		/// 
		/// @throws AssetLoaderNotFoundException si el cargador para el tipo
		/// de asset no ha sido previamente regsitrado.
		void LaunchAsyncLoad(
			const std::string& assetPath, 
			IAssetLoader* loader,
			std::span<const std::string> tags);

		std::unordered_map<std::string, UniquePtr<IAssetLoader>, StringHasher, std::equal_to<>> m_loaders;

	};

}
