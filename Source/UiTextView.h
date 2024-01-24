#pragma once

#include "UiElement.h"

#include "AssetRef.h"
#include "Font.h"

namespace OSK::ASSETS {
	class Font;
}

namespace OSK::UI {

	/// @brief Elemento de UI que permite mostrar un texto.
	/// Para su uso se deben establecer tanto una fuente como su tama�o.
	class OSKAPI_CALL TextView : public IElement {

	public:

		explicit TextView(const Vector2f& size) : IElement(size) { }

	public:

		/// @brief Ajusta el tama�o ocupado por el elemento
		/// para que cubra todo el texto y nada m�s que el texto.
		void AdjustSizeToText();


		/// @brief Establece el tama�o de fuente que se usar�.
		/// @param size Tama�o de la fuente, en p�xeles.
		void SetFontSize(USize32 size);

		/// @brief Establece la fuente que se usar� para el renderizado del texto.
		/// @param font Fuente del texto.
		void SetFont(ASSETS::AssetRef<ASSETS::Font> font);


		/// @return Fuente usada por el text view.
		/// Puede ser null.
		const ASSETS::Font* GetFont() const;

		/// @return Tama�o de fuente usado por el text view.
		USize32 GetFontSize() const;


		/// @brief Establece el texto que se va a renderizar.
		/// @param text Texto renderizado.
		void SetText(const std::string& text);

		/// @return Texto renderizado.
		std::string_view GetText() const;

		/// @note No renderizar� el texto al no ser que se haya establecido la fuente.
		void Render(GRAPHICS::SpriteRenderer* renderer, Vector2f parentPosition) const override;

	private:

		std::string text = "";
		ASSETS::AssetRef<ASSETS::Font> font;
		USize32 fontSize = 12;

	};

}
