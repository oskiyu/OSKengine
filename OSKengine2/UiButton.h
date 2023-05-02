#pragma once

#include "UiElement.h"

#include "UiImageView.h"
#include "UiTextView.h"

#include <functional>

namespace OSK::UI {

	/// @brief Elemento de UI que permite al usuario interaccionar.
	class OSKAPI_CALL Button : public IElement {

	public:

		/// @brief Estado en el que está en botón en un
		/// momento dado.
		enum class State {

			/// @brief Estado inicial.
			DEFAULT,

			/// @brief Estado en el que está cuando
			/// el usuario sobrepasa el botón con
			/// el ratón.
			SELECTED,

			/// @brief Estado en el que está cuando
			/// se pulsa el botón.
			PRESSED

		};

		/// @brief Tipo de botón, que condiciona su comportamiento.
		enum class Type {

			/// @brief Botón normal.
			NORMAL,

			/// @brief Botón estilo TOGGLE.
			TOGGLE

		};

		/// @brief Callback que se ejecuta al pulsar el botón.
		/// @param isPressed Indica si el nuevo estado es activo o no.
		/// Para botones normales, siempre será true.
		/// Para bottones estilo TOGGLE, indicará si a partir de la pulsación
		/// el botón está a true o a false.
		using CallbackFnc = std::function<void(bool isPressed)>;

	public:

		/// @brief Crea el botón.
		/// @param size Tamaño del botón.
		/// 
		/// @note Por defecto, no tendrá texto.
		/// @note Por defecto, no tendrá textura.
		Button(const Vector2f size);

		/// @brief Crea el botón.
		/// @param size Tamaño del botón.
		/// @param text Texto del botón.
		/// 
		/// @note No se renderizará el texto hasta que no se establezca una 
		/// fuente mediante SetTextFont().
		/// @note Por defecto, no tendrá textura.
		Button(const Vector2f size, const std::string& text);


		/// @brief Simula un click del usuario.
		void Click();

		/// @brief Renderiza el botón de acuerdo a su estado actual.
		/// Si se renderiza el texto, estará centrado.
		/// @see TextView.Render() para precondiciones.
		/// @see ImageView.Render() para precondiciones.
		void Render(GRAPHICS::SpriteRenderer* renderer, Vector2f parentPosition) const override;

		void UpdateByCursor(Vector2f cursor, bool isPressed, Vector2f parentPosition) override;

		/// @brief Establece el callback que se ejecutará cuando
		/// el usuario pulse el botón.
		/// @param callback Función que se ejecutará.
		void SetCallback(CallbackFnc callback);

		/// @brief Sobreescribe el estado acutal del botón.
		/// @param state Nuevo estado.
		void SetState(State state);

		/// @brief Establece el tipo de botón.
		/// @param type Nuevo tipo de botón.
		void SetType(Type type);

		/// @return Devuelve el tipo de botón actual.
		Type GetType() const;

	public:

		/// @brief Establece la fuente del texto.
		/// @param font Fuente.
		/// 
		/// @see TextView.SetFont() para precondiciones y postcondiciones.
		void SetTextFont(ASSETS::Font* font);

		/// @brief Establece el tamaño de la fuente del texto.
		/// @param fontSize Tamaño, en píxeles.
		/// 
		/// @see TextView.SetFontSize() para precondiciones y postcondiciones.
		void SetTextFontSize(TSize fontSize);

		/// @brief Establece el texto a renderizar.
		/// El texto será renderizado centrado.
		/// @param text Nuevo texto.
		void SetText(const std::string& text);

		/// @return Texto del botón.
		std::string_view GetText() const;

	public:

		/// @return Sprite que se renderizará si tiene el estado por defecto.
		GRAPHICS::Sprite& GetDefaultSprite();
		/// @return Sprite que se renderizará si tiene el estado por defecto.
		const GRAPHICS::Sprite& GetDefaultSprite() const;

		/// @return Sprite que se renderizará si está siendo seleccionado.
		GRAPHICS::Sprite& GetSelectedSprite();
		/// @return Sprite que se renderizará si está siendo seleccionado.
		const GRAPHICS::Sprite& GetSelectedSprite() const;
		
		/// @return Sprite que se renderizará si está siendo pulsado.
		GRAPHICS::Sprite& GetPressedSprite();
		/// @return Sprite que se renderizará si está siendo pulsado.
		const GRAPHICS::Sprite& GetPressedSprite() const;

		/// @param state Estado del botón.
		/// @return Sprite que se renderizará en el estado dado.
		GRAPHICS::Sprite& GetSprite(State state);
		/// @param state Estado del botón.
		/// @return Sprite que se renderizará en el estado dado.
		const GRAPHICS::Sprite& GetSprite(State state) const;

	private:

		void _SetRelativePosition(const Vector2f& relativePosition) override;

		Vector2f GetTextRelativePosition() const;

	private:

		/// @return ImageView que se renderizará con el estado actual.
		const ImageView& GetCurrentImage() const;

		State currentState = State::DEFAULT;
		bool wasPreviousFramePressed = false;

		bool currentToggleState = false;

		Type type = Type::TOGGLE;

		ImageView defaultImage;
		ImageView selectedImage;
		ImageView pressedImage;

		TextView buttonText;

		CallbackFnc callback = [](bool){};

	};

}
