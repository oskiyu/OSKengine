#pragma once

#include "OSKmacros.h"

#include <json.hpp>

#include "HashMap.hpp"
#include "UniquePtr.hpp"
#include "AssetRef.h"
#include <map>
#include "AssetOwningRef.h"
#include <string>

#include "AssetsMap.h"

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

		/// @brief Realiza el proceso de carga el asset.
		/// @param assetFilePath Ruta del archivo de descripción que describe el asset.
		/// @param asset Puntero al asset a cargar.
		/// 
		/// @pre El asset debe haber sido previamente registrado mediante RegisterWithoutLoading().
		/// @pre El asset no debe haber sido previamente cargado, ya sea mediante Load() o
		/// mediante FullyLoad().
		/// 
		/// @throws AssetDescriptionFileNotFoundException Si el archivo de descripción '.json' no existe.
		/// @throws InvalidDescriptionFileException Si el archivo de descripción es inválido.
		/// @throws RawAssetFileNotFoundException Si el archivo del asset original no se encuentra.
		virtual void Load(const std::string& assetFilePath) = 0;

		/// @brief Realiza el proceso completo de registro de asset y carga del asset.
		/// @param assetFilePath Ruta del archivo de descripción que describe el asset.
		/// @param assetRef Puntero a la referencia del asset.
		/// 
		/// @pre @p *assetRef debe apuntar a una referencia ya creada.
		/// 
		/// @throws AssetDescriptionFileNotFoundException Si el archivo de descripción '.json' no existe.
		/// @throws InvalidDescriptionFileException Si el archivo de descripción es inválido.
		/// @throws RawAssetFileNotFoundException Si el archivo del asset original no se encuentra.
		virtual void FullyLoad(const std::string& assetFilePath, void* assetRef) = 0;

		/// @brief Registra el asset en la tabla de assets, creando la AssetOwningRef.
		/// @param assetFilePath Ruta del archivo de descripción que describe el asset.
		/// @param assetRef Puntero a la referencia del asset.
		/// 
		/// @pre @p *assetRef debe apuntar a una referencia ya creada.
		virtual void RegisterWithoutLoading(const std::string& assetFilePath, void* assetRef) = 0;

		/// @brief Establece el flag indicando que el asset ha sido comletamente cargado.
		/// @param assetFilePath Ruta del archivo de descripción que describe el asset.
		/// 
		/// @pre El asset debe haber sido previamente registrado (ya sea mediante FullyLoad()
		/// o mediante RegisterWithoutLoading()).
		virtual void SetAsLoaded(std::string_view assetFilePath) = 0;

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


	/// @brief Clase base que contiene la tabña caché
	/// de assets previamente cargados.
	/// @tparam TAssetType Tipo de asset.
	template <typename TAssetType>
	class TAssetLoader {

	public:

		virtual ~TAssetLoader() = default;

		virtual void Load(const std::string& path, TAssetType* asset) = 0;

	protected:

		AssetsMap<TAssetType> m_assetsTable;

	};

}

#define OSK_DEFAULT_LOADER_IMPL(TType) \
void RegisterWithoutLoading(const std::string& assetFilePath, void* assetRef) override { \
	OSK::ASSETS::AssetRef<TType>& output = *static_cast<OSK::ASSETS::AssetRef<TType>*>(assetRef); \
	output = m_assetsTable.GetReference_OrRegister(assetFilePath); \
} \
\
void FullyLoad(const std::string& assetFilePath, void* assetRef) override { \
	OSK::ASSETS::AssetRef<TType>& output = *static_cast<OSK::ASSETS::AssetRef<TType>*>(assetRef); \
	if (!m_assetsTable.HasAsset(assetFilePath)) { \
		m_assetsTable.Register(assetFilePath); \
		Load(assetFilePath, m_assetsTable.GetAsset(assetFilePath));\
	} \
	output = m_assetsTable.GetExistingReference(assetFilePath); \
} \
void SetAsLoaded(std::string_view assetFilePath) { \
	m_assetsTable.SetAsLoaded(assetFilePath); \
} \
void Load(const std::string& assetFilePath) override { \
	Load(assetFilePath, m_assetsTable.GetAsset(assetFilePath)); \
	SetAsLoaded(assetFilePath); \
}

// OSK::ASSETS::AssetRef<TType>& output = *static_cast<OSK::ASSETS::AssetRef<TType>*>(assetRef); \
