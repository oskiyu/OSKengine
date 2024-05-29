#pragma once

#include "DynamicArray.hpp"

#include <span>
#include <string>

// Para std::is_base_of_v
#include <type_traits> 

namespace OSK {

	namespace UI {
		class Console;
	}


	/// @brief Representa un comando de consola
	/// que se puede ejecutar.
	class OSKAPI_CALL IConsoleCommand {

	public:

		virtual ~IConsoleCommand() = default;

		/// @brief Ejecuta el comando.
		/// @param console Consola sobre la que se mandó el mensaje.
		/// @param params Parámetros del comando.
		/// @pre Los parámetros (@p params) deben ser las
		/// palabras del mensaje una vez se han
		/// quitado las palabras obligatorias del comando.
		virtual void Execute(
			UI::Console* console,
			std::span<const std::string> params) const = 0;

		/// @brief Comrpueba si las palabras de un
		/// mensaje son compatibles con este comando.
		/// @param messageWords Palabras sueltas del mensaje,
		/// en orden.
		/// @return True si es compatible con este comando
		/// (por lo que se debe ejecutar este comando).
		bool IsCompatibleWith(std::span<const std::string> messageWords) const;

		/// @return Palabras sueltas y ordenadas obligatorias del comando.
		std::span<const std::string> GetCommandWords() const;

	protected:

		/// @brief Establece el texto que debe tener el mensaje
		/// para ser considerado este comando.
		/// 
		/// Se insertan las palabras sueltas del comando,
		/// en orden.
		/// @param words Palabras que componen la parte obligatoria
		/// del comando.
		/// 
		/// @pre @p words.GetSize() > 0.
		/// @throws InvalidArgumentException si @p words.GetSize() == 0.
		void SetCommandWords(const DynamicArray<std::string>& words);
		void SetCommandWords(DynamicArray<std::string>&& words);

	private:

		/// @brief Texto que debe tener un mensaje
		/// para corresponderse con este comando.
		/// Palabras separadas.
		DynamicArray<std::string> m_commandText;

	};


	/// @brief Concepto para clases que son comandos de consola.
	template <typename T>
	concept IsConsoleCommand =
		std::is_base_of_v<IConsoleCommand, T>;

}
