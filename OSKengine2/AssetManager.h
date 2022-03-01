#pragma once

#include "OSKmacros.h"
#include "LinkedList.hpp"
#include "HashMap.hpp"
#include "SharedPtr.hpp"
#include "IAssetLoader.h"

namespace OSK {

	class IAsset;

	/// <summary>
	/// El AssetManager se encarga de manejar los loaders de los assets y los lifetimes
	/// de los assets.
	/// 
	/// Se deben registrar todos los loaders mediante RegisterLoader().
	/// 
	/// Al cargar un asset mediante Load(), debemos especificar a qué lifetime corresponde.
	/// Un lifetime agrupa assets que tienen un ciclo de vida común. Al terminar el ciclo de
	/// vida, todos los assets del lifetime son eliminados.
	/// </summary>
	class AssetManager {

	public:

		~AssetManager();

		/// <summary>
		/// Carga un asset.
		/// </summary>
		template <typename T> T* Load(const std::string& assetFilePath, const std::string lifetimePool) {
			if (assetsTable.ContainsKey(assetFilePath))
				return (T*)assetsTable.Get(assetFilePath);

			T* output = new T(assetFilePath);
			loaders.Get(T::GetAssetType())->Load(assetFilePath, (IAsset**)&output);

			assetsTable.Insert(output->GetName(), output);

			if (!assetsPerLifetime.ContainsKey(lifetimePool))
				assetsPerLifetime.Insert(lifetimePool, {});

			assetsPerLifetime.Get(lifetimePool).Insert(output);

			return output;
		}

		/// <summary>
		/// Elimina todos assets del lifetime dado.
		/// </summary>
		void DeleteLifetime(const std::string& lifetime);

		/// <summary>
		/// Registra un loader, para poder cargar assets de un tipo determinado.
		/// T debe ser el tipo de asset, NO el loader.
		/// </summary>
		template <typename T> void RegisterLoader() {
			loaders.Insert(T::GetAssetType(), new T);
		}

	private:

		HashMap<std::string, IAsset*> assetsTable;
		HashMap<std::string, DynamicArray<SharedPtr<IAsset>>> assetsPerLifetime;
		HashMap<std::string, IAssetLoader*> loaders;

	};

}
