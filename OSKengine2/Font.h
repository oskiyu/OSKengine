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
	/// debemos cargar una fuente de manera independiente por cada tama�o
	/// de letra que queramos usar.
	/// 
	/// @note Considere precargar los tama�os de letra que se vayan a usar
	/// m�s adelante, para evitar p�rdidas de rendimiento durante la ejecuci�n.
	/// 
	/// @warning �nicamente soporta caracteres ASCII.
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
		/// @warning Funci�n interna: no llamar.
		/// 
		/// <param name="rawFile">
		/// Ruta del archivo '.tff'. Est� establecido en el archivo '.json' del asset.
		/// </param>
		void _SetFontFilePath(const std::string& rawFile);

		/// <summary>
		/// Genera la imagen de la fuente para el tama�o de letra dado.
		/// </summary>
		/// <param name="size">Tama�o de letra, en p�xeles.</param>
		void LoadSizedFont(TSize size);

		/// <summary>
		/// Devuelve la imagen de GPU correspondiente a la fuente del tama�o dado.
		/// </summary>
		/// 
		/// @note Si la fuente no est� cargada, se cargar� autom�ticamente.
		/// 
		/// <param name="fontSize">Tama�o de la fuente, en p�xeles.</param>
		GRAPHICS::GpuImage* GetGpuImage(TSize fontSize);

		/// <summary>
		/// Devuelve la informaci�n de un car�cter para un tama�o de fuente dado.
		/// </summary>
		/// 
		/// @note Si la fuente no est� cargada, se cargar� autom�ticamente.
		/// 
		/// <param name="size">Tama�o de la fuente, en p�xeles.</param>
		/// <param name="character">Caracter.</param>
		const FontCharacter& GetCharacterInfo(TSize size, char character);

		/// <summary>
		/// Devuelve la informaci�n de una instancia de la fuente con el tama�o dado.
		/// </summary>
		/// 
		/// @note Si la fuente no est� cargada, se cargar� autom�ticamente.
		/// 
		/// <param name="fontSize">Tama�o de la fuente.</param>
		const FontInstance& GetInstance(TSize fontSize);

	private:

		HashMap<TSize, FontInstance> instances;
		std::string fontFile;

	};

}
