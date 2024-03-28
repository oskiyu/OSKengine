#pragma once

#include <string>

#include "HashMap.hpp"
#include "OSKmacros.h"
#include "LinkedList.hpp"
#include "SharedPtr.hpp"
#include "UniquePtr.hpp"
#include "IAssetLoader.h"
#include "IAsset.h"
#include "Logger.h"
#include "AssetRef.h"

#include "AssetLoaderNotFoundException.h"

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

		/// <summary> Carga un asset. </summary>
		/// 
		/// @note Esta función debe ser sobreescrita por todos los loaders.
		template <typename T> 
		AssetRef<T> Load(const std::string& assetFilePath) {
			AssetRef<T> output = AssetRef<T>();

			OSK_ASSERT(m_loaders.contains(T::GetAssetType()), AssetLoaderNotFoundException(T::GetAssetType()))

			m_loaders.at(T::GetAssetType())->Load(assetFilePath, &output);

			return output;
		}

		/// <summary> Registra un loader, para poder cargar assets de un tipo determinado. </summary>
		/// 
		/// @pre No debe haberse registrado antes este loader.
		/// 
		/// @warning T debe ser el tipo de asset, NO el loader.
		/// @warning Todo loader debe ser registrado para poder usarse.
		template <typename T> 
		void RegisterLoader() {
			const auto& key = T::GetAssetType();

			OSK_ASSERT(!m_loaders.contains(key), AssetLoaderAlreadyRegisteredException(key))
			m_loaders[T::GetAssetType()] = new T;
		}

		/// @brief Devuelve el loader indicado.
		/// @tparam TLoader Loader a obtener.
		/// @return Loader.
		/// @pre El loader debe haber sido previamente registrado.
		template <typename TLoader>
		TLoader* GetLoader() {
			const auto& key = TLoader::GetAssetType();

			OSK_ASSERT(m_loaders.contains(TLoader::GetAssetType()), AssetLoaderNotFoundException(TLoader::GetAssetType()))

			return static_cast<TLoader*>(m_loaders.find(key)->second.GetPointer());
		}

	private:

		std::unordered_map<std::string, UniquePtr<IAssetLoader>, StringHasher, std::equal_to<>> m_loaders;

	};

}
