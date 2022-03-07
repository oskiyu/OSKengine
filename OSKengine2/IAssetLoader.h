#pragma once

#include "OSKmacros.h"

#include <string>

#ifndef OSK_ASSET_TYPE_REG
#define OSK_ASSET_TYPE_REG(typeName) const static std::string GetAssetType() { return typeName; }
#endif

namespace OSK::ASSETS {

	class IAsset;

	/// <summary>
	/// Cada tipo de asset debe tener su loader espec�fico.
	/// Cada asset loader tiene que ser registrado manualmente en el AssetManager de OSKengine.
	/// 
	/// Al igual que con Asset, debe especificarse a qu� tipo de Asset corresponde, usando
	/// el macro OSK_ASSET_TYPE_REG. El string debe ser el mismo que el especificado en el Asset.
	/// </summary>
	class OSKAPI_CALL IAssetLoader {

	public:

		/// <summary>
		/// Carga el asset.
		/// </summary>
		/// <param name="assetFilePath">Ruta del archivo .json que describe el asset.</param>
		/// <param name="asset">Puntero al puntero del asset a cargar.</param>
		virtual void Load(const std::string& assetFilePath, IAsset** asset) = 0;

	};

}
