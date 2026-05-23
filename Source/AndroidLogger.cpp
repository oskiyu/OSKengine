#include "AndroidLogger.h"

#ifdef OSK_ANDROID

#include "LoggerExceptions.h"

#include <android/log.h>

#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace OSK;
using namespace OSK::IO;

AndroidLogger::~AndroidLogger() {
	try {
		Close();
	}
	catch (const EngineException&) {}
}

void AndroidLogger::Start(const std::string& path) {
	m_path = path;
	m_hasBeenStarted = true;
}

void AndroidLogger::_Log(LogLevel level, const std::string& msg) {
	std::lock_guard lock(m_mutex);

	if (!m_hasBeenStarted)
		throw LoggerNotInitializedException();

	std::string start = "";
	auto androidLevel = ANDROID_LOG_UNKNOWN;

	switch (level) {
	case LogLevel::INFO:
		start = "INFO: ";
		androidLevel = ANDROID_LOG_INFO;
		break;

	case LogLevel::WARNING:
		start = "WARNING: ";
		androidLevel = ANDROID_LOG_WARN;
		break;

	case LogLevel::L_ERROR:
		start = "ERROR: ";
		androidLevel = ANDROID_LOG_ERROR;
		break;

	case LogLevel::L_DEBUG:
		start = "DEBUG: ";
		androidLevel = ANDROID_LOG_DEBUG;
		break;
	}

	__android_log_print(androidLevel, "OSKENGINE", "%s %s", start.c_str(), msg.data());

	m_output.append(start);
	m_output.append(msg);

	m_output.append("\n");
}

void AndroidLogger::DebugLog(const std::string& message) {
	Log(LogLevel::L_DEBUG, message);
}
void AndroidLogger::InfoLog(const std::string& message) {
	Log(LogLevel::INFO, message);
}

void AndroidLogger::Save() {
	Save(m_path);
}

void AndroidLogger::Save(const std::string& filename) {
	return;
}

void AndroidLogger::Clear() {
	if (!m_hasBeenStarted)
		throw LoggerNotInitializedException();

	m_output.clear();
}

void AndroidLogger::Close() {
	if (!m_hasBeenStarted)
		throw LoggerNotInitializedException();

	InfoLog("Cerrado el log.");

	Save();
	Clear();
}

#endif // OSK_ANDROID
