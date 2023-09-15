#pragma once

#include "IAsset.h"
#include "OwnedPtr.h"
#include "IGpuImage.h"
#include "Vector4.hpp"
#include "HashMap.hpp"
#include "FontInstance.h"
#include "FontCharacter.h"

namespace OSK::GRAPHICS {
	class Material;
}

namespace OSK::ASSETS {

	/// <summary> Representa una fuente que nos permite renderizar texto. </summary>
	/// 
	/// @note Usa una estrategia de cración perezosa:
	/// debemos cargar una fuente de manera independiente por cada tamaño
	/// de letra que queramos usar.
	/// 
	/// @note Considere precargar los tamaños de letra que se vayan a usar
	/// más adelante, para evitar pérdidas de rendimiento durante la ejecución.
	/// 
	/// @warning Únicamente soporta caracteres ASCII.
	class OSKAPI_CALL Font : public IAsset {

	public:

		OSK_ASSET_TYPE_REG("OSK::Font");

		OSK_DISABLE_COPY(Font);
		OSK_DEFAULT_MOVE_OPERATOR(Font);

		Font(const std::string& assetFile);

		/// <summary>
		/// Establece la ruta de la fuente (.tff), para poder
		/// generar las fuentes.
		/// </summary>
		/// 
		/// @warning Función interna: no llamar.
		/// 
		/// <param name="rawFile">
		/// Ruta del archivo '.tff'. Está establecido en el archivo '.json' del asset.
		/// </param>
		void _SetFontFilePath(const std::string& rawFile);

		/// <summary> Genera la imagen de la fuente para el tamaño de letra dado. </summary>
		/// <param name="size">Tamaño de letra, en píxeles.
		/// Si ya existe una instancia cargada con el tamaño dado,
		/// no ocurre nada.</param>
		/// 
		/// @throws FontLibraryInitializationException Si ocurre un error al cargar la librería de generación de fuentes.
		/// @throws FontLodaingException Si ocurre un error al cargar la fuente.
		/// @throws FontCharacterLodaingException Si ocurre un error al cargar un carácter en concreto.
		void LoadSizedFont(USize32 size);

		/// <summary> Descarga la instancia de la funente con el tamaño dado. </summary>
		/// <param name="size">Tamaño de letra, en píxeles.</param>
		/// 
		/// @note Si la instancia no existe, no ocurre nada.
		void UnloadSizedFont(USize32 size);

		/// <summary> Devuelve la imagen de GPU correspondiente a la fuente del tamaño dado. </summary>
		/// <param name="fontSize">Tamaño de la fuente, en píxeles.</param>
		/// 
		/// @note Si la fuente no está cargada, se cargará automáticamente.
		GRAPHICS::GpuImage* GetGpuImage(USize32 fontSize);

		/// <summary> Devuelve la información de un carácter para un tamaño de fuente dado. </summary>
		/// 
		/// @note Si la fuente no está cargada, se cargará automáticamente.
		/// 
		/// <param name="size">Tamaño de la fuente, en píxeles.</param>
		/// <param name="character">Caracter.</param>
		const FontCharacter& GetCharacterInfo(USize32 size, char character);

		/// <summary> Devuelve la información de una instancia de la fuente con el tamaño dado. </summary>
		/// 
		/// @note Si la fuente no está cargada, se cargará automáticamente.
		/// 
		/// <param name="fontSize">Tamaño de la fuente.</param>
		/// 
		/// @throws FontLibraryInitializationException Si ocurre un error al cargar la librería de generación de fuentes.
		/// @throws FontLodaingException Si ocurre un error al cargar la fuente.
		/// @throws FontCharacterLodaingException Si ocurre un error al cargar un carácter en concreto.
		FontInstance& GetInstance(USize32 fontSize);

		/// @brief Devuelve la información de una instancia de la fuente con el tamaño dado.
		/// @param fontSize Tamaño de la fuente.
		/// @return Instancia de la fuente.
		/// 
		/// @pre Se debe haber cargado previamente una instancia con el tamaño dado.
		const FontInstance& GetExistingInstance(USize32 fontSize) const;

		/// @param fontSize Tamaño de la fuente.
		/// @return True si se ha cargado previamente una instancia con el tamaño dado.
		bool ContainsInstance(USize32 fontSize) const;

		/// <summary>
		/// Establece el material a partir del que se van a crear
		/// las instancias de materiales para las instancias de fuentes.
		/// </summary>
		void SetMaterial(GRAPHICS::Material* material);

		/// <summary>
		/// Devuelve el material del que se crean
		/// las instancias.
		/// </summary>
		/// 
		/// @note Puede devolver null.
		GRAPHICS::Material* GetMaterial() const;

	private:

		std::unordered_map<USize32, FontInstance> m_instances;
		std::string m_fontFile;
		GRAPHICS::Material* m_material = nullptr;

	};

}
