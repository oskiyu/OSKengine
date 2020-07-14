#pragma once

#include "OSKmacros.h"

#include "LogMessageLevelsEnum.h"

#include <iostream>
#include <string>

namespace OSK {

	OSK_INFO_STATIC
	//Clase para Log.
	class OSKAPI_CALL Logger {

	public:

		//Inicializa el Logger.
		//	<path>: directorio en el que se guarda el log (modo RELEASE).
		static void Start(const std::string& path = "");

		//Muestra el mensaje...
		//DEBUG: por la consola.
		//RELEASE: en LOG_LAST.txt.
		static void Log(const LogMessageLevels& level, const std::string&, const int& = -1);


		//Muestra el mensaje por la consola.
		//DEBUG ONLY.
		static void DebugLog(const std::string&, const int& = -1);


		//Cierra el Logger.
		static void Close();


		//Guarda el Logger (modo RELEASE).
		static void Save(const std::string& filename = "LOG_LAST.txt");


		//Limpia el Logger (modo RELEASE).
		static void Clear();

	private:

		static bool hasBeenStarted;

#ifdef OSK_RELEASE
		static std::string output;
#endif

		static std::string path;

	};

}