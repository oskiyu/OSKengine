#pragma once

#include "OSKmacros.h"

#include "IToken.h"

#include <string>
#include <vector>

namespace OSK::SceneSystem::Loader {

	/// <summary>
	/// Clase que implementa funciones para poder procesar archivos de escena (.sc).
	/// </summary>
	class OSKAPI_CALL Scanner {

	public:

		/// <summary>
		/// Elimina el scanner.
		/// </summary>
		~Scanner();

		/// <summary>
		/// Devuelve una lista con todos los tokens del script dato.
		/// </summary>
		/// <param name="code">Script de la escena.</param>
		/// <returns>Tokens.</returns>
		std::vector<IToken*>& GetTokens(const std::string& code);

		/// <summary>
		/// Devuelve la palabra siguiente en el script.
		/// </summary>
		std::string ProcessText();

		/// <summary>
		/// Devuelve el int siguiente en el script.
		/// </summary>
		int ProcessInteger();

		/// <summary>
		/// Devuelve el float siguiente en el script.
		/// </summary>
		float ProcessFloat();

		/// <summary>
		/// Devuelve el próximo string entre ( . . . ).
		/// </summary>
		std::string ProcessDataToken();

		/// <summary>
		/// True si el char es un número (o .).
		/// </summary>
		static bool IsNumber(char c);

		/// <summary>
		/// True si el char es un espacio (o equivalente).
		/// </summary>
		static bool IsSpace(char c);

		/// <summary>
		/// Limpia el código y los tokens.
		/// </summary>
		void Clear();

	private:

		/// <summary>
		/// Devuelve el próximo char.
		/// </summary>
		char PeekNextChar() const;

		/// <summary>
		/// Añade el próximo token del script.
		/// </summary>
		void AddNextToken();

		/// <summary>
		/// Tokens.
		/// </summary>
		std::vector<IToken*> tokens;

		/// <summary>
		/// Contenido del archivo.
		/// </summary>
		std::string sourceCode;

		/// <summary>
		/// Apunta al caracter que se está leyendo actualmente.
		/// </summary>
		size_t currentChar = 0;

		/// <summary>
		/// Línea que se está leyendo ahora mismo.
		/// </summary>
		size_t linea = 0;

	};

}