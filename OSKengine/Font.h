#pragma once

#include <map>
#include "FontChar.h"

#include "MaterialInstance.h"

namespace OSK {

	/// <summary>
	/// Fuente para el renderizado de texto.
	/// </summary>
	struct OSKAPI_CALL Font {

		friend class ContentManager;
		friend class SpriteBatch;

	public:

		/// <summary>
		/// Espacios a los que equivale '\t'.
		/// </summary>
		const static int SPACES_PER_TAB = 4;
		
		/// <summary>
		/// Obtiene el tamaño de un texto que se renderice con esta fuente.
		/// </summary>
		/// <param name="texto">Texto.</param>
		/// <param name="size">Tamaño de la fuente.</param>
		/// <returns>Tamaño del texto.</returns>
		Vector2 GetTextSize(const std::string& texto, const Vector2& size) const;

		/// <summary>
		/// Tamaño de la fuente.
		/// </summary>
		uint32_t GetFontSize() const {
			return size;
		}

	private:

		/// <summary>
		/// Caracteres de la fuente.
		/// </summary>
		std::map<char, FontChar> characters;

		/// <summary>
		/// Tamaño de la fuente.
		/// </summary>
		uint32_t size;

		/// <summary>
		/// Material de la fuente, para su renderizado.
		/// </summary>
		SharedPtr<MaterialInstance> fontMaterial;

	};

}