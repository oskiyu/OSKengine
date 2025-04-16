#pragma once

#include "ApiCall.h"

#include "UniquePtr.hpp"
#include "DynamicArray.hpp"

#include "ConsoleCommand.h"

#include <string>
#include <string_view>

namespace OSK {

	namespace UI {
		class Console;
	}


	/// @brief Clase que ejecuta comandos de consola.
	class OSKAPI_CALL ConsoleCommandExecutor {

	public:

		/// @brief Registra un comando para poder
		/// ser ejecutado.
		/// @tparam TCommand Tipo de comando.
		/// 
		/// @pre @p TCommand debe ser un tipo
		/// de comando (heredando de IConsoleCommand).
		/// @pre El comando @p TCommand no debe haber
		/// sido previamente registrado.
		template <typename TCommand> requires IsConsoleCommand<TCommand>
		void RegisterCommand() {
			RegisterCommand(MakeUnique<TCommand>());
		}

		/// @brief Registra un comando para poder
		/// ser ejecutado.
		/// @param command Comando a registrar.
		/// @pre El comando @p command no debe haber
		/// sido previamente registrado.
		void RegisterCommand(UniquePtr<IConsoleCommand>&& command);


		/// @brief Comprueba si el texto se corresponde
		/// con algún comando, en cuyo caso lo ejecuta.
		/// @param console Consola sobre la que se mandó el mensaje.
		/// @param text Texto del mensaje.
		void CheckAndExecute(
			UI::Console* console,
			std::string_view text) const;

	private:

		/// @brief Divide el mensaje en una lista de palabras.
		/// Cada palabra en el texto original está separada por espacios
		/// y tabulaciones.
		/// @param message Mensaje a separar.
		/// @return Lista de palabras.
		DynamicArray<std::string> SplitMessage(std::string_view message) const;

		DynamicArray<UniquePtr<IConsoleCommand>> m_registeredCommands;

	};

}
