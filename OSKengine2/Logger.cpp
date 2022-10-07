#include "Logger.h"

#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace OSK;
using namespace OSK::IO;

Logger::~Logger() {
	Close();
}

void Logger::Start(const std::string& path) {
	this->path = path;
	hasBeenStarted = true;
}

void Logger::Log(LogLevel level, const std::string& msg) {
	if (!hasBeenStarted)
		throw std::runtime_error("OSK::Logger no ha sido inicializado. Usa Logger::Start() antes.");

	std::string start = "";

	switch (level) {
	case LogLevel::INFO:
		start = "INFO: ";
		break;

	case LogLevel::WARNING:
		start = "WARNING: ";
		break;

	case LogLevel::L_ERROR:
		start = "ERROR: ";
		break;

	case LogLevel::L_DEBUG:
		start = "DEBUG: ";
		break;
	}

//#ifdef OSK_DEBUG
	std::cout << start << msg << std::endl;
//#else
	output.append(start);
	output.append(msg);

	output.append("\n");
//#endif

}

void Logger::DebugLog(const std::string& message) {
	Log(LogLevel::L_DEBUG, message);
}
void Logger::InfoLog(const std::string& message) {
	Log(LogLevel::INFO, message);
}

void Logger::Save() {
	Save(path);
}

void Logger::Save(const std::string& filename) {
	if (!hasBeenStarted)
		throw std::runtime_error("OSK::Logger no ha sido inicializado. Usa Logger::Start() antes.");

//#ifdef OSK_RELEASE
	std::ofstream outputLog(path + filename);

	if (outputLog.is_open())
		outputLog << output << std::endl;
	
	outputLog.close();
//#endif // OSK_RELEASE
}

void Logger::Clear() {
	if (!hasBeenStarted)
		throw std::runtime_error("OSK::Logger no ha sido inicializado. Usa Logger::Start() antes.");

	output.clear();
}

void Logger::Close() {
	if (!hasBeenStarted)
		throw std::runtime_error("OSK::Logger no ha sido inicializado. Usa Logger::Start() antes.");

	InfoLog("Cerrado el log.");

	Save();
	Clear();
}
