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
		explicit Button(const Vector2f size);

		/// @brief Crea el bot�n.
		/// @param size Tama�o del bot�n.
		/// @param text Texto del bot�n.
		/// 
		/// @note No se renderizar� el texto hasta que no se establezca una 
		/// fuente mediante SetTextFont().
		/// @note Por defecto, no tendr� textura.
		Button(const Vector2f size, const std::string& text);


		void SetSize(Vector2f size) override;


		/// @brief Simula un click del usuario.
		void Click();

		/// @brief Renderiza el bot�n de acuerdo a su estado actual.
		/// Si se renderiza el texto, estar� centrado.
		/// @see TextView.Render() para precondiciones.
		/// @see ImageView.Render() para precondiciones.
		void Render(GRAPHICS::SdfBindlessRenderer2D* renderer) const override;

		bool UpdateByCursor(Vector2f cursor, bool isPressed) override;

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
		void SetTextFont(ASSETS::AssetRef<ASSETS::Font> font);

		/// @brief Establece el tama�o de la fuente del texto.
		/// @param fontSize Tama�o, en p�xeles.
		/// 
		/// @see TextView.SetFontSize() para precondiciones y postcondiciones.
		void SetTextFontSize(USize32 fontSize);

		/// @brief Establece el texto a renderizar.
		/// El texto ser� renderizado centrado.
		/// @param text Nuevo texto.
		void SetText(const std::string& text);

		/// @return Texto del bot�n.
		std::string_view GetText() const;

	public:

		DynamicArray<GRAPHICS::SdfDrawCall2D>& GetDefaultDrawCalls();
		const DynamicArray<GRAPHICS::SdfDrawCall2D>& GetDefaultDrawCalls()const;

		/// @return Sprite que se renderizar� si est� siendo seleccionado.
		DynamicArray<GRAPHICS::SdfDrawCall2D>& GetSelectedDrawCalls();
		/// @return Sprite que se renderizar� si est� siendo seleccionado.
		const DynamicArray<GRAPHICS::SdfDrawCall2D>& GetSelectedDrawCalls() const;
		
		/// @return Sprite que se renderizar� si est� siendo pulsado.
		DynamicArray<GRAPHICS::SdfDrawCall2D>& GetPressedDrawCalls();
		/// @return Sprite que se renderizar� si est� siendo pulsado.
		const DynamicArray<GRAPHICS::SdfDrawCall2D>& GetPressedDrawCalls() const;

		/// @param state Estado del bot�n.
		/// @return Sprite que se renderizar� en el estado dado.
		DynamicArray<GRAPHICS::SdfDrawCall2D>& GetDrawCalls(State state);
		/// @param state Estado del bot�n.
		/// @return Sprite que se renderizar� en el estado dado.
		const DynamicArray<GRAPHICS::SdfDrawCall2D>& GetDrawCalls(State state) const;

	private:

		void _SetPosition(const Vector2f& newPosition) override;
		Vector2f GetTextRelativePosition() const;

	private:

		State m_currentState = State::DEFAULT;
		bool m_wasPreviousFramePressed = false;

		bool m_currentToggleState = false;

		Type m_type = Type::TOGGLE;

		ImageView m_defaultImage;
		ImageView m_selectedImage;
		ImageView m_pressedImage;

		TextView m_buttonText;

		CallbackFnc m_callback = [](bool) {  (void)0; };

	};

}
