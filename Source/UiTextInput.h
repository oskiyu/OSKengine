#pragma once

#include "UiElement.h"

#include "AssetRef.h"
#include "Font.h"


namespace OSK::ASSETS {
	class Font;
}


namespace OSK::UI {

	/// @brief Elemento de UI que permite mostrar un texto.
	/// Para su uso se deben establecer tanto una fuente como su tamaño.
	class OSKAPI_CALL TextInput : public IElement {

	public:

		explicit TextInput(const Vector2f& size) : IElement(size) { }

	public:

		/// @brief Establece el tamaño de fuente que se usará.
		/// @param size Tamaño de la fuente, en píxeles.
		void SetFontSize(USize32 size);

		/// @brief Establece la fuente que se usará para el renderizado del texto.
		/// @param font Fuente del texto.
		void SetFont(ASSETS::AssetRef<ASSETS::Font> font);


		/// @brief Establece el texto que se va a renderizar.
		/// @param text Texto renderizado.
		void SetText(const std::string& text);

		/// @brief Establece el texto por defecto, que será
		/// reemplazado una vez se escriba algo.
		/// @param text Texto.
		void SetPlaceholderText(const std::string& text);


		/// @return Texto renderizado.
		std::string_view GetText() const;

		/// @return True si el texto que cointiene es el 
		/// por defecto.
		bool HasPlaceholderText() const;

		void UpdateByKeyboard(const IO::KeyboardState& previous, const IO::KeyboardState& current) override;
		void Render(GRAPHICS::ISdfRenderer2D* renderer) const override;

	private:

		bool m_hasPlaceholderText = false;

		std::string m_text = "";

		ASSETS::AssetRef<ASSETS::Font> m_font;
		USize32 m_fontSize = 12;

	};

}
