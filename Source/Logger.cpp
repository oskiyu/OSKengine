#include "Logger.h"

#include "LoggerExceptions.h"

#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace OSK;
using namespace OSK::IO;

Logger::~Logger() {
	try {
		Close();
	}
	catch (const EngineException&) {}
}

void Logger::Start(const std::string& path) {
	m_path = path;
	m_hasBeenStarted = true;
}

void Logger::Log(LogLevel level, const std::string& msg) {
	std::lock_guard lock(m_mutex);

	if (!m_hasBeenStarted)
		throw LoggerNotInitializedException();

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

	std::cout << start << msg << std::endl;

	m_output.append(start);
	m_output.append(msg);

	m_output.append("\n");
}

void Logger::DebugLog(const std::string& message) {
	Log(LogLevel::L_DEBUG, message);
}
void Logger::InfoLog(const std::string& message) {
	Log(LogLevel::INFO, message);
}

void Logger::Save() {
	Save(m_path);
}

void Logger::Save(const std::string& filename) {
	if (!m_hasBeenStarted)
		throw LoggerNotInitializedException();

//#ifdef OSK_RELEASE
	std::ofstream outputLog(m_path + filename);

	if (outputLog.is_open())
		outputLog << m_output << std::endl;
	
	outputLog.close();
//#endif // OSK_RELEASE
}

void Logger::Clear() {
	if (!m_hasBeenStarted)
		throw LoggerNotInitializedException();

	m_output.clear();
}

void Logger::Close() {
	if (!m_hasBeenStarted)
		throw LoggerNotInitializedException();

	InfoLog("Cerrado el log.");

	Save();
	Clear();
}
