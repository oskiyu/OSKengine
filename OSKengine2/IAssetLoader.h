#pragma once

#include "OSKmacros.h"

#include <json.hpp>

#include <string>

#ifndef OSK_ASSET_TYPE_REG
#define OSK_ASSET_TYPE_REG(typeName) const static std::string GetAssetType() { return typeName; }
#endif

namespace OSK::ASSETS {

	class IAsset;

	/// <summary>
	/// Clase que se encarga de cargar un asset de un tipo en concreto.
	/// </summary>
	/// 
	/// @note Cada tipo de asset debe tener su loader específico.
	/// @warning Cada asset loader tiene que ser registrado manualmente en el AssetManager de OSKengine.
	/// 
	/// @warning Al igual que con Asset, debe especificarse a qué tipo de Asset corresponde, usando
	/// el macro OSK_ASSET_TYPE_REG. El string debe ser el mismo que el especificado en el Asset.
	class OSKAPI_CALL IAssetLoader {

	public:

		virtual ~IAssetLoader() = default;

		/// @brief Carga el asset-
		/// @param assetFilePath Ruta del archivo de descripción que describe el asset.
		/// @param asset Puntero al puntero del asset a cargar.
		/// 
		/// @pre IAsset** asset debe apuntar a un asset ya creado (no puede ser null).
		/// 
		/// @throws AssetDescriptionFileNotFoundException Si el archivo de descripción '.json' no existe.
		/// @throws InvalidDescriptionFileException Si el archivo de descripción es inválido.
		/// @throws RawAssetFileNotFoundException Si el archivo del asset original no se encuentra.
		virtual void Load(const std::string& assetFilePath, IAsset** asset) = 0;

	protected:

		/// @brief Valida el contenido del archivo de descripción.
		/// @param filePath Ruta al archivo completo de descripción.
		/// @return Archivo de desscripción, si fue validado.
		/// 
		/// @throws AssetDescriptionFileNotFoundException Si el archivo de descripción '.json' no existe.
		/// @throws InvalidDescriptionFileException Si el archivo de descripción es inválido.
		/// @throws RawAssetFileNotFoundException Si el archivo del asset original no se encuentra.
		nlohmann::json ValidateDescriptionFile(std::string_view filePath) const;

	};

}
