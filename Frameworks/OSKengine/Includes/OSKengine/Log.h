#pragma once

#include "OSKmacros.h"

#include "LogMessageLevelsEnum.h"

#include <iostream>
#include <string>
#include <stdexcept>

namespace OSK {

	/// <summary>
	/// Clase para Log.
	/// </summary>
	class OSKAPI_CALL Logger {

	public:

		/// <summary>
		/// Inicializa el Logger.
		/// </summary>
		/// <param name="path">Directorio en el que se guarda el log (modo RELEASE).</param>
		static void Start(const std::string& path = "");

		/// <summary>
		/// Muestra el mensaje...
		/// DEBUG: por la consola.
		/// RELEASE: en LOG_LAST.txt.
		/// </summary>
		/// <param name="level">TIpo de mensaje.</param>
		/// <param name="string">Mensaje.</param>
		/// <param name="">Línea de código.</param>
		static void Log(const LogMessageLevels& level, const std::string&, int = -1);

		/// <summary>
		/// Muestra el mensaje por la consola.
		/// DEBUG ONLY.
		/// </summary>
		/// <param name="string">Mensaje.</param>
		/// <param name="">Línea de código.</param>
		static void DebugLog(const std::string& message, int line = -1);

		/// <summary>
		/// Cierra el Logger.
		/// </summary>
		static void Close();

		/// <summary>
		/// Guarda el Logger (modo RELEASE).
		/// </summary>
		/// <param name="filename">Línea de código.</param>
		static void Save(const std::string& filename = "LOG_LAST.txt");

		/// <summary>
		/// Limpia el Logger (modo RELEASE).
		/// </summary>
		static void Clear();

	private:



		/// <summary>
		/// True si ha sido iniciado.
		/// </summary>
		static bool hasBeenStarted;

#ifdef OSK_RELEASE
		/// <summary>
		/// Fichero de salida.
		/// </summary>
		static std::string output;
#endif

		/// <summary>
		/// Output.
		/// </summary>
		static std::string path;

	};

}
	