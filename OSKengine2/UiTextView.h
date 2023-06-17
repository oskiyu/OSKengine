#pragma once

#include "UiElement.h"

namespace OSK::ASSETS {
	class Font;
}

namespace OSK::UI {

	/// @brief Elemento de UI que permite mostrar un texto.
	/// Para su uso se deben establecer tanto una fuente como su tama�o.
	class OSKAPI_CALL TextView : public IElement {

	public:

		TextView(const Vector2f& size) : IElement(size) { }

	public:

		/// @brief Ajusta el tama�o ocupado por el elemento
		/// para que cubra todo el texto y nada m�s que el texto.
		void AdjustSizeToText();


		/// @brief Establece el tama�o de fuente que se usar�.
		/// @param size Tama�o de la fuente, en p�xeles.
		void SetFontSize(USize32 size);

		/// @brief Establece la fuente que se usar� para el renderizado del texto.
		/// @param font Fuente del texto.
		/// 
		/// @pre @p font no debe ser null.
		void SetFont(ASSETS::Font* font);


		/// @brief Establece el texto que se va a renderizar.
		/// @param text Texto renderizado.
		void SetText(const std::string& text);

		/// @return Texto renderizado.
		std::string_view GetText() const;

		/// @note No renderizar� el texto al no ser que se haya establecido la fuente.
		void Render(GRAPHICS::SpriteRenderer* renderer, Vector2f parentPosition) const override;

	private:

		std::string text = "";
		ASSETS::Font* font = nullptr;
		USize32 fontSize = 12;

	};

}
