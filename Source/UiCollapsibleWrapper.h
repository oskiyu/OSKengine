#pragma once

#include "UiVerticalContainer.h"
#include "UiButton.h"
#include "AssetRef.h"

namespace OSK::GRAPHICS {
	class SdfBindlessRenderer2D;
}

namespace OSK::IO {
	class KeyboardState;
}

namespace OSK::ASSETS {
	class Font;
}

namespace OSK::UI {

	/// @brief Contenedor de un �nico elemento que permite
	/// contraerlo y expandirlo.
	/// Tambi�n contiene un bot�n para efectuar el cambio.
	class OSKAPI_CALL CollapsibleWrapper : public VerticalContainer {

	public:

		/// @brief Crea el contenedor.
		/// @param content Contenido del contenedor.
		/// @param title T�tulo del bot�n.
		/// @param font Fuente del bot�n.
		/// @param fontSize Tama�o de fuente del bot�n.
		explicit CollapsibleWrapper(
			OwnedPtr<IElement> content,
			const std::string& title,
			ASSETS::AssetRef<ASSETS::Font> font,
			USize32 fontSize);


		/// @return Contenido contra�ble del contenedor.
		IElement* GetContent();
		const IElement* GetContent() const;


		/// @brief Expande el contenido,
		/// de tal manera que sea visible.
		/// 
		/// @post `GetContent()` ser� vissible.
		/// @post `GetSize().y == GetToggleBar()->GetSize().y + GetContent().GetSize().y + GetContent().GetMargin2D().y`.
		void Expand();

		/// @brief Contrae el contenido,
		/// de tal manera que no sea visible.
		/// 
		/// @post `GetContent()` ser� invisible.
		/// @post `GetSize().y == GetToggleBar()->GetSize().y`.
		void Collapse();


		/// @return Bot�n que efect�a el cambio entre
		/// ambos estados.
		Button* GetToggleBar();
		const Button* GetToggleBar() const;


		/// @brief Ajusta el tama�o horizontal
		/// del bot�n para que sea igual al del 
		/// contenido.
		void AdjustToggleBarSize();

	private:

		Button* m_bar;
		IElement* m_content;

	};

}
