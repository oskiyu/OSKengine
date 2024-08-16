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

	/// @brief Contenedor de un único elemento que permite
	/// contraerlo y expandirlo.
	/// También contiene un botón para efectuar el cambio.
	class OSKAPI_CALL CollapsibleWrapper : public VerticalContainer {

	public:

		/// @brief Crea el contenedor.
		/// @param content Contenido del contenedor.
		/// @param title Título del botón.
		/// @param font Fuente del botón.
		/// @param fontSize Tamaño de fuente del botón.
		explicit CollapsibleWrapper(
			OwnedPtr<IElement> content,
			const std::string& title,
			ASSETS::AssetRef<ASSETS::Font> font,
			USize32 fontSize);


		/// @return Contenido contraíble del contenedor.
		IElement* GetContent();
		const IElement* GetContent() const;


		/// @brief Expande el contenido,
		/// de tal manera que sea visible.
		/// 
		/// @post `GetContent()` será vissible.
		/// @post `GetSize().y == GetToggleBar()->GetSize().y + GetContent().GetSize().y + GetContent().GetMargin2D().y`.
		void Expand();

		/// @brief Contrae el contenido,
		/// de tal manera que no sea visible.
		/// 
		/// @post `GetContent()` será invisible.
		/// @post `GetSize().y == GetToggleBar()->GetSize().y`.
		void Collapse();


		/// @return Botón que efectúa el cambio entre
		/// ambos estados.
		Button* GetToggleBar();
		const Button* GetToggleBar() const;


		/// @brief Ajusta el tamaño horizontal
		/// del botón para que sea igual al del 
		/// contenido.
		void AdjustToggleBarSize();

	private:

		Button* m_bar;
		IElement* m_content;

	};

}
