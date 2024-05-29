#pragma once

#include "UiElement.h"

#include "UiTextView.h"
#include "UiTextInput.h"
#include "UiVerticalContainer.h"


namespace OSK::UI {

	/// @brief Consola del motor, que permite ejecutar comandos.
	/// Muestra los 20 últimos mensajes.
	class OSKAPI_CALL Console : public VerticalContainer {

	public:

		constexpr static auto Name = "OSK::EngineConsole";

		explicit Console(const Vector2f& size);


		void SetFont(ASSETS::AssetRef<ASSETS::Font> font);
		void SetFontSize(USize32 fontSize);

		/// @brief Añade un mensaje a la consola.
		/// @param msg Mensaje a añadir.
		void AddMessage(const std::string& msg);

		void UpdateByKeyboard(const IO::KeyboardState& previous, const IO::KeyboardState& current) override;

	private:

		/// @brief Número máximo de mensajes almacenados
		/// (y que se muestran).
		constexpr static USize32 MaxMessages = 20;

		/// @brief Input sobre el que escribe el desarrollador.
		/// @note No será null.
		TextInput* m_textInput = nullptr;

		/// @brief Array con los views de los mensajes.
		/// Después de crearse la consola, estarán todos
		/// creados, pero invisibles.
		/// Se harán haciendo visibles a medida que se
		/// añadan mensajes.
		TextView* m_textMessages[MaxMessages];

		/// @brief Índice del próximo view
		/// que se hará visible tras insertar
		/// un mensaje.
		/// @post Será siempre <= MaxMessages.
		UIndex32 m_nextMessageIndex = 0;

	};

}
