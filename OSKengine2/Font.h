#pragma once

#include "IAsset.h"
#include "OwnedPtr.h"
#include "IGpuImage.h"
#include "Vector4.hpp"
#include "HashMap.hpp"
#include "FontInstance.h"
#include "FontCharacter.h"

namespace OSK::ASSETS {

	/// <summary>
	/// Representa una fuente: un asset que almacena una fuente que nos
	/// permite renderizar texto.
	/// 
	/// @note Al cargar una fuente, en realidad no se carga la fuente:
	/// debemos cargar una fuente de manera independiente por cada tamaño
	/// de letra que queramos usar.
	/// 
	/// @note Considere precargar los tamaños de letra que se vayan a usar
	/// más adelante, para evitar pérdidas de rendimiento durante la ejecución.
	/// 
	/// @warning Únicamente soporta caracteres ASCII.
	/// </summary>
	class OSKAPI_CALL Font : public IAsset {

	public:

		OSK_ASSET_TYPE_REG("OSK::Font");

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

		/// <summary>
		/// Genera la imagen de la fuente para el tamaño de letra dado.
		/// </summary>
		/// <param name="size">Tamaño de letra, en píxeles.</param>
		void LoadSizedFont(TSize size);

		/// <summary>
		/// Devuelve la imagen de GPU correspondiente a la fuente del tamaño dado.
		/// </summary>
		/// 
		/// @note Si la fuente no está cargada, se cargará automáticamente.
		/// 
		/// <param name="fontSize">Tamaño de la fuente, en píxeles.</param>
		GRAPHICS::GpuImage* GetGpuImage(TSize fontSize);

		/// <summary>
		/// Devuelve la información de un carácter para un tamaño de fuente dado.
		/// </summary>
		/// 
		/// @note Si la fuente no está cargada, se cargará automáticamente.
		/// 
		/// <param name="size">Tamaño de la fuente, en píxeles.</param>
		/// <param name="character">Caracter.</param>
		const FontCharacter& GetCharacterInfo(TSize size, char character);

		/// <summary>
		/// Devuelve la información de una instancia de la fuente con el tamaño dado.
		/// </summary>
		/// 
		/// @note Si la fuente no está cargada, se cargará automáticamente.
		/// 
		/// <param name="fontSize">Tamaño de la fuente.</param>
		const FontInstance& GetInstance(TSize fontSize);

	private:

		HashMap<TSize, FontInstance> instances;
		std::string fontFile;

	};

}
