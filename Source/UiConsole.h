#pragma once

#include "UiElement.h"

#include "UiTextView.h"
#include "UiTextInput.h"
#include "UiVerticalContainer.h"


namespace OSK::UI {

	/// @brief Consola del motor, que permite ejecutar comandos.
	/// Muestra los 20 �ltimos mensajes.
	class OSKAPI_CALL Console : public VerticalContainer {

	public:

		constexpr static auto Name = "OSK::EngineConsole";

		explicit Console(const Vector2f& size);


		void SetFont(ASSETS::AssetRef<ASSETS::Font> font);
		void SetFontSize(USize32 fontSize);

		/// @brief A�ade un mensaje a la consola.
		/// @param msg Mensaje a a�adir.
		void AddMessage(const std::string& msg);

		void UpdateByKeyboard(const IO::KeyboardState& previous, const IO::KeyboardState& current) override;

	private:

		/// @brief N�mero m�ximo de mensajes almacenados
		/// (y que se muestran).
		constexpr static USize32 MaxMessages = 20;

		/// @brief Input sobre el que escribe el desarrollador.
		/// @note No ser� null.
		TextInput* m_textInput = nullptr;

		/// @brief Array con los views de los mensajes.
		/// Despu�s de crearse la consola, estar�n todos
		/// creados, pero invisibles.
		/// Se har�n haciendo visibles a medida que se
		/// a�adan mensajes.
		TextView* m_textMessages[MaxMessages];

		/// @brief �ndice del pr�ximo view
		/// que se har� visible tras insertar
		/// un mensaje.
		/// @post Ser� siempre <= MaxMessages.
		UIndex32 m_nextMessageIndex = 0;

	};

}
