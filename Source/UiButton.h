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
		explicit Button(const Vector2f size);

		/// @brief Crea el botón.
		/// @param size Tamaño del botón.
		/// @param text Texto del botón.
		/// 
		/// @note No se renderizará el texto hasta que no se establezca una 
		/// fuente mediante SetTextFont().
		/// @note Por defecto, no tendrá textura.
		Button(const Vector2f size, const std::string& text);


		void SetSize(Vector2f size) override;


		/// @brief Simula un click del usuario.
		void Click();

		/// @brief Renderiza el botón de acuerdo a su estado actual.
		/// Si se renderiza el texto, estará centrado.
		/// @see TextView.Render() para precondiciones.
		/// @see ImageView.Render() para precondiciones.
		void Render(GRAPHICS::SdfBindlessRenderer2D* renderer) const override;

		bool UpdateByCursor(Vector2f cursor, bool isPressed) override;

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
		void SetTextFont(ASSETS::AssetRef<ASSETS::Font> font);

		/// @brief Establece el tamaño de la fuente del texto.
		/// @param fontSize Tamaño, en píxeles.
		/// 
		/// @see TextView.SetFontSize() para precondiciones y postcondiciones.
		void SetTextFontSize(USize32 fontSize);

		/// @brief Establece el texto a renderizar.
		/// El texto será renderizado centrado.
		/// @param text Nuevo texto.
		void SetText(const std::string& text);

		/// @return Texto del botón.
		std::string_view GetText() const;

	public:

		DynamicArray<GRAPHICS::SdfDrawCall2D>& GetDefaultDrawCalls();
		const DynamicArray<GRAPHICS::SdfDrawCall2D>& GetDefaultDrawCalls()const;

		/// @return Sprite que se renderizará si está siendo seleccionado.
		DynamicArray<GRAPHICS::SdfDrawCall2D>& GetSelectedDrawCalls();
		/// @return Sprite que se renderizará si está siendo seleccionado.
		const DynamicArray<GRAPHICS::SdfDrawCall2D>& GetSelectedDrawCalls() const;
		
		/// @return Sprite que se renderizará si está siendo pulsado.
		DynamicArray<GRAPHICS::SdfDrawCall2D>& GetPressedDrawCalls();
		/// @return Sprite que se renderizará si está siendo pulsado.
		const DynamicArray<GRAPHICS::SdfDrawCall2D>& GetPressedDrawCalls() const;

		/// @param state Estado del botón.
		/// @return Sprite que se renderizará en el estado dado.
		DynamicArray<GRAPHICS::SdfDrawCall2D>& GetDrawCalls(State state);
		/// @param state Estado del botón.
		/// @return Sprite que se renderizará en el estado dado.
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
