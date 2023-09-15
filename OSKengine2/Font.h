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
	/// @note Usa una estrategia de craci�n perezosa:
	/// debemos cargar una fuente de manera independiente por cada tama�o
	/// de letra que queramos usar.
	/// 
	/// @note Considere precargar los tama�os de letra que se vayan a usar
	/// m�s adelante, para evitar p�rdidas de rendimiento durante la ejecuci�n.
	/// 
	/// @warning �nicamente soporta caracteres ASCII.
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
		/// @warning Funci�n interna: no llamar.
		/// 
		/// <param name="rawFile">
		/// Ruta del archivo '.tff'. Est� establecido en el archivo '.json' del asset.
		/// </param>
		void _SetFontFilePath(const std::string& rawFile);

		/// <summary> Genera la imagen de la fuente para el tama�o de letra dado. </summary>
		/// <param name="size">Tama�o de letra, en p�xeles.
		/// Si ya existe una instancia cargada con el tama�o dado,
		/// no ocurre nada.</param>
		/// 
		/// @throws FontLibraryInitializationException Si ocurre un error al cargar la librer�a de generaci�n de fuentes.
		/// @throws FontLodaingException Si ocurre un error al cargar la fuente.
		/// @throws FontCharacterLodaingException Si ocurre un error al cargar un car�cter en concreto.
		void LoadSizedFont(USize32 size);

		/// <summary> Descarga la instancia de la funente con el tama�o dado. </summary>
		/// <param name="size">Tama�o de letra, en p�xeles.</param>
		/// 
		/// @note Si la instancia no existe, no ocurre nada.
		void UnloadSizedFont(USize32 size);

		/// <summary> Devuelve la imagen de GPU correspondiente a la fuente del tama�o dado. </summary>
		/// <param name="fontSize">Tama�o de la fuente, en p�xeles.</param>
		/// 
		/// @note Si la fuente no est� cargada, se cargar� autom�ticamente.
		GRAPHICS::GpuImage* GetGpuImage(USize32 fontSize);

		/// <summary> Devuelve la informaci�n de un car�cter para un tama�o de fuente dado. </summary>
		/// 
		/// @note Si la fuente no est� cargada, se cargar� autom�ticamente.
		/// 
		/// <param name="size">Tama�o de la fuente, en p�xeles.</param>
		/// <param name="character">Caracter.</param>
		const FontCharacter& GetCharacterInfo(USize32 size, char character);

		/// <summary> Devuelve la informaci�n de una instancia de la fuente con el tama�o dado. </summary>
		/// 
		/// @note Si la fuente no est� cargada, se cargar� autom�ticamente.
		/// 
		/// <param name="fontSize">Tama�o de la fuente.</param>
		/// 
		/// @throws FontLibraryInitializationException Si ocurre un error al cargar la librer�a de generaci�n de fuentes.
		/// @throws FontLodaingException Si ocurre un error al cargar la fuente.
		/// @throws FontCharacterLodaingException Si ocurre un error al cargar un car�cter en concreto.
		FontInstance& GetInstance(USize32 fontSize);

		/// @brief Devuelve la informaci�n de una instancia de la fuente con el tama�o dado.
		/// @param fontSize Tama�o de la fuente.
		/// @return Instancia de la fuente.
		/// 
		/// @pre Se debe haber cargado previamente una instancia con el tama�o dado.
		const FontInstance& GetExistingInstance(USize32 fontSize) const;

		/// @param fontSize Tama�o de la fuente.
		/// @return True si se ha cargado previamente una instancia con el tama�o dado.
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
