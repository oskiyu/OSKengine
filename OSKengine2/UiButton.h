#pragma once

#include "UiElement.h"

#include "UiImageView.h"
#include "UiTextView.h"

#include <functional>

namespace OSK::UI {

	/// @brief Elemento de UI que permite al usuario interaccionar.
	class OSKAPI_CALL Button : public IElement {

	public:

		/// @brief Estado en el que est� en bot�n en un
		/// momento dado.
		enum class State {

			/// @brief Estado inicial.
			DEFAULT,

			/// @brief Estado en el que est� cuando
			/// el usuario sobrepasa el bot�n con
			/// el rat�n.
			SELECTED,

			/// @brief Estado en el que est� cuando
			/// se pulsa el bot�n.
			PRESSED

		};

		/// @brief Tipo de bot�n, que condiciona su comportamiento.
		enum class Type {

			/// @brief Bot�n normal.
			NORMAL,

			/// @brief Bot�n estilo TOGGLE.
			TOGGLE

		};

		/// @brief Callback que se ejecuta al pulsar el bot�n.
		/// @param isPressed Indica si el nuevo estado es activo o no.
		/// Para botones normales, siempre ser� true.
		/// Para bottones estilo TOGGLE, indicar� si a partir de la pulsaci�n
		/// el bot�n est� a true o a false.
		using CallbackFnc = std::function<void(bool isPressed)>;

	public:

		/// @brief Crea el bot�n.
		/// @param size Tama�o del bot�n.
		/// 
		/// @note Por defecto, no tendr� texto.
		/// @note Por defecto, no tendr� textura.
		Button(const Vector2f size);

		/// @brief Crea el bot�n.
		/// @param size Tama�o del bot�n.
		/// @param text Texto del bot�n.
		/// 
		/// @note No se renderizar� el texto hasta que no se establezca una 
		/// fuente mediante SetTextFont().
		/// @note Por defecto, no tendr� textura.
		Button(const Vector2f size, const std::string& text);


		/// @brief Simula un click del usuario.
		void Click();

		/// @brief Renderiza el bot�n de acuerdo a su estado actual.
		/// Si se renderiza el texto, estar� centrado.
		/// @see TextView.Render() para precondiciones.
		/// @see ImageView.Render() para precondiciones.
		void Render(GRAPHICS::SpriteRenderer* renderer, Vector2f parentPosition) const override;

		void UpdateByCursor(Vector2f cursor, bool isPressed, Vector2f parentPosition) override;

		/// @brief Establece el callback que se ejecutar� cuando
		/// el usuario pulse el bot�n.
		/// @param callback Funci�n que se ejecutar�.
		void SetCallback(CallbackFnc callback);

		/// @brief Sobreescribe el estado acutal del bot�n.
		/// @param state Nuevo estado.
		void SetState(State state);

		/// @brief Establece el tipo de bot�n.
		/// @param type Nuevo tipo de bot�n.
		void SetType(Type type);

		/// @return Devuelve el tipo de bot�n actual.
		Type GetType() const;

	public:

		/// @brief Establece la fuente del texto.
		/// @param font Fuente.
		/// 
		/// @see TextView.SetFont() para precondiciones y postcondiciones.
		void SetTextFont(ASSETS::Font* font);

		/// @brief Establece el tama�o de la fuente del texto.
		/// @param fontSize Tama�o, en p�xeles.
		/// 
		/// @see TextView.SetFontSize() para precondiciones y postcondiciones.
		void SetTextFontSize(TSize fontSize);

		/// @brief Establece el texto a renderizar.
		/// El texto ser� renderizado centrado.
		/// @param text Nuevo texto.
		void SetText(const std::string& text);

		/// @return Texto del bot�n.
		std::string_view GetText() const;

	public:

		/// @return Sprite que se renderizar� si tiene el estado por defecto.
		GRAPHICS::Sprite& GetDefaultSprite();
		/// @return Sprite que se renderizar� si tiene el estado por defecto.
		const GRAPHICS::Sprite& GetDefaultSprite() const;

		/// @return Sprite que se renderizar� si est� siendo seleccionado.
		GRAPHICS::Sprite& GetSelectedSprite();
		/// @return Sprite que se renderizar� si est� siendo seleccionado.
		const GRAPHICS::Sprite& GetSelectedSprite() const;
		
		/// @return Sprite que se renderizar� si est� siendo pulsado.
		GRAPHICS::Sprite& GetPressedSprite();
		/// @return Sprite que se renderizar� si est� siendo pulsado.
		const GRAPHICS::Sprite& GetPressedSprite() const;

		/// @param state Estado del bot�n.
		/// @return Sprite que se renderizar� en el estado dado.
		GRAPHICS::Sprite& GetSprite(State state);
		/// @param state Estado del bot�n.
		/// @return Sprite que se renderizar� en el estado dado.
		const GRAPHICS::Sprite& GetSprite(State state) const;

	private:

		void _SetRelativePosition(const Vector2f& relativePosition) override;

		Vector2f GetTextRelativePosition() const;

	private:

		/// @return ImageView que se renderizar� con el estado actual.
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
