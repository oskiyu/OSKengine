#pragma once

#include "OSKmacros.h"

namespace OSK::SceneSystem::Loader {

	/// <summary>
	/// Tipos de token para procesar un script de escena.
	/// </summary>
	enum class TokenType {

		/// <summary>
		/// Token que representa la versión del archivo.
		/// </summary>
		VERSION,

		/// <summary>
		/// Carga un skybox.
		/// </summary>
		SKYBOX,

		/// <summary>
		/// Carga un heightmap.
		/// </summary>
		TERRAIN,

		/// <summary>
		/// Spawnea un game object.
		/// </summary>
		PLACE,

		/// <summary>
		/// Token desconocido.
		/// </summary>
		UNKNOWN

	};

	class Scanner;

	/// <summary>
	/// Token del script de escena.
	/// </summary>
	class OSKAPI_CALL IToken {

	public:

		virtual ~IToken() = default;

		/// <summary>
		/// Procesa la información necesaria para el token.
		/// </summary>
		/// <param name="scanner">Scanner que está leyendo el token.</param>
		virtual void ProcessData(Scanner* scanner) {

		}

		/// <summary>
		/// Tipo de token.
		/// </summary>
		TokenType tokenType = TokenType::UNKNOWN;

	};

}
