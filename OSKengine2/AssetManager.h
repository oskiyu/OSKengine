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
		template <typename T> T* 
		Load(const std::string& assetFilePath, const std::string& lifetimePool) {
			if (assetsTable.contains(assetFilePath))
				return (T*)assetsTable.at(assetFilePath);

			T* output = new T(assetFilePath);

			OSK_ASSERT(loaders.contains(T::GetAssetType()), AssetLoaderNotFoundException(T::GetAssetType()))

			loaders.at(T::GetAssetType())->Load(assetFilePath, (IAsset**)&output);

			assetsTable[output->GetName()] = output;

			if (!assetsPerLifetime.contains(lifetimePool))
				assetsPerLifetime[lifetimePool] = {};

			assetsPerLifetime.at(lifetimePool).Insert(output);

			return output;
		}

		/// <summary> Elimina todos assets del lifetime dado. </summary>
		/// 
		/// @note Si el lifetime no existe (o ya fue eliminado), no ocurre nada.
		void DeleteLifetime(std::string_view lifetime);

		/// <summary> Registra un loader, para poder cargar assets de un tipo determinado. </summary>
		/// 
		/// @pre No debe haberse registrado antes este loader.
		/// 
		/// @warning T debe ser el tipo de asset, NO el loader.
		/// @warning Todo loader debe ser registrado para poder usarse.
		template <typename T> 
		void RegisterLoader() {
			loaders[T::GetAssetType()] = new T;
		}

	private:

		std::unordered_map<std::string, IAsset*, StringHasher, std::equal_to<>> assetsTable;
		std::unordered_map<std::string, DynamicArray<SharedPtr<IAsset>>, StringHasher, std::equal_to<>> assetsPerLifetime;
		std::unordered_map<std::string, UniquePtr<IAssetLoader>, StringHasher, std::equal_to<>> loaders;

	};

}
