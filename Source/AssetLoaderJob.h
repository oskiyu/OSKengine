#pragma once

#include "IJob.h"

namespace OSK::ASSETS {

	class IAssetLoader;

	/// @brief Trabajo que se encarga de cargar un asset
	/// de manera diferida.
	class AssetLoaderJob : public IJob {

	public:

		OSK_JOB("OSK::AssetLoaderJob");

		/// @brief Establece los parámetros del trabajo.
		/// @param assetPath Ruta del archivo de descripción.
		/// @param loader Loader que debe cargar el asset.
		/// 
		/// @pre @p loader debe ser un puntero estable.
		AssetLoaderJob(
			const std::string& assetPath, 
			IAssetLoader* loader);

		void Execute() override;

	private:

		std::string m_assetPath;
		IAssetLoader* m_loader = nullptr;

	};

}
