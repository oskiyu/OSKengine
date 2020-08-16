#pragma once

#include "OSKmacros.h"

#include "LogMessageLevelsEnum.h"

#include <iostream>
#include <string>
#include <stdexcept>

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

#define OSK_GET_TRACE std::string("\tat: ") + std::string(__FUNCTION__) +  ", line " + std::to_string(__LINE__)
#define OSK_SHOW_TRACE() OSK::Logger::Log(OSK::LogMessageLevels::NO, OSK_GET_TRACE)
#define OSK_SHOW_ERROR(what) OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, std::string(what) + "\n" + OSK_GET_TRACE)
#define OSK_SHOW_WARNING(what) OSK::Logger::Log(OSK::LogMessageLevels::WARNING, std::string(what) + "\n" +OSK_GET_TRACE)
			
//#define OSK_CHECK(x) if (x != OSK::OskResult::SUCCESS) { OSK_SHOW_TRACE(); }
//#define OSK_CHECK_RET(x) if (x != OSK::OskResult::SUCCESS) { OSK_SHOW_TRACE(); return x; }

}