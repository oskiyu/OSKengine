#include "Log.h"

#include <iostream>
#include <fstream>

namespace OSK {

	bool Logger::hasBeenStarted = false;

#ifdef OSK_RELEASE
	std::string Logger::output = "";
#endif // OSK_RELEASE

	std::string Logger::path = "";


	OSK_INFO_STATIC
	void Logger::Start(const std::string& targetPath) {
		path = targetPath;

		hasBeenStarted = true;
#ifdef OSK_RELEASE
		output = "";
#endif // OSK_RELEASE

	}


	OSK_INFO_STATIC 
	void Logger::Log(const LogMessageLevels& level, const std::string& msg, const int& line) {
		if (!hasBeenStarted)
			throw std::runtime_error("OSK::Logger no ha sido inicializado. Usa Logger::Start() antes.");
		
		std::string start = "";

		switch (level) {
			case LogMessageLevels::INFO:
				start = "INFO: ";
				break;

			case LogMessageLevels::WARNING:
				start = "WARNING: ";
				break;

			case LogMessageLevels::BAD_ERROR:
				start = "ERROR: ";
				break;

			case LogMessageLevels::CRITICAL_ERROR:
				start = "CRITICAL ERROR: ";
				break;

			case LogMessageLevels::DEBUG:
#ifndef OSK_DEBUG
				return;
#endif
				start = "DEBUG: ";
				break;

		}

#ifdef OSK_DEBUG

		if (line == -1) {
			std::cout << start << msg << std::endl;
		}
		else {
			std::cout << start << msg << "; linea: " << line << std::endl;
		}
#else
		output.append(start);
		output.append(msg);

		if (line != -1)
			output.append("; linea: " + std::to_string(line));

		output.append("\n");
#endif
	}


	OSK_INFO_STATIC
	void Logger::Save(const std::string& filename) {
		if (!hasBeenStarted)
			throw std::runtime_error("OSK::Logger no ha sido inicializado. Usa Logger::Start() antes.");

#ifdef OSK_RELEASE
		std::ofstream outputLog(path + filename);
		if (outputLog.is_open()) {
			outputLog << output << std::endl;
		}
		outputLog.close();
#endif
	}


	OSK_INFO_STATIC
	void Logger::Clear() {
		if (!hasBeenStarted)
			throw std::runtime_error("OSK::Logger no ha sido inicializado. Usa Logger::Start() antes.");

#ifdef OSK_RELEASE
		output.clear();
#endif
	}


	OSK_INFO_STATIC
	void Logger::Close() {
		if (!hasBeenStarted)
			throw std::runtime_error("OSK::Logger no ha sido inicializado. Usa Logger::Start() antes.");

		Save();
#ifdef OSK_RELEASE
		Clear();
#endif
	}


	OSK_INFO_STATIC
	void Logger::DebugLog(const std::string& message, const int& line) {
#ifdef OSK_DEBUG
		Log(LogMessageLevels::DEBUG, message, line);
#endif
	}

}
