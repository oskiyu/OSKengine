#pragma once

#include "OSKmacros.h"

#include <string>

#undef ERROR

namespace OSK {

	/// <summary>
	/// Nivel de importancia del mensaje.
	/// </summary>
	enum class LogLevel {
		/// <summary>
		/// Sin ningún nivel establecido.
		/// </summary>
		NO,
		/// <summary>
		/// Mensaje informativo.
		/// </summary>
		INFO,
		/// <summary>
		/// Mensaje de depuración.
		/// </summary>
		L_DEBUG,
		/// <summary>
		/// Mensaje de advertencia importante.
		/// </summary>
		WARNING,
		/// <summary>
		/// Mensaje de error irrecuperable.
		/// </summary>
		L_ERROR
	};

	/// <summary>
	/// Clase que permite enviar mensajes por consola, o por
	/// donde se defina su salida.
	/// 
	/// También guarda el resultado del log.
	/// 
	/// Puede usarse el logger del motor desde Engine::GetLogger().
	/// </summary>
	class OSKAPI_CALL Logger {

	public:

		/// <summary>
		/// Cierra el logger, guardando el archivo.
		/// </summary>
		~Logger();

		/// <summary>
		/// Inicializa el logger.
		/// </summary>
		/// <param name="path">Ruta donde se guardará el archivo de log.</param>
		void Start(const std::string& path);

		/// <summary>
		/// Manda un mensaje por el logger.
		/// </summary>
		void Log(LogLevel level, const std::string& msg);

		/// <summary>
		/// Manda un mensaje de depuración.
		/// </summary>
		void DebugLog(const std::string& msg);

		/// <summary>
		/// Manda un mensaje de información.
		/// </summary>
		void InfoLog(const std::string& msg);

		/// <summary>
		/// Guarda el archivo del log, en la ruta especificada
		/// en Logger::Start().
		/// </summary>
		void Save();

		/// <summary>
		/// Guarda el archivo del log, en la ruta especificada.
		/// </summary>
		void Save(const std::string& path);

		/// <summary>
		/// Limpia los contenidos del archivo del log.
		/// </summary>
		void Clear();

		/// <summary>
		/// Cierra el logger, guardando el archivo.
		/// </summary>
		void Close();

	private:

		// Contenido del archivo que se va a escribir.
		std::string output;
		std::string path;

		bool hasBeenStarted = false;

	};

}
