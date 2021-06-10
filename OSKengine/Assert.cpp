#include "Assert.h"

#include "Log.h"

using namespace OSK;

void OSK::RuntimeAssertIsTrueFunction(bool condition, const std::string& msg, const std::string& functionName, uint32_t line, const std::string& file) {
	if (!condition) {
		std::string message = "Assertion failed: " + msg + "\n";
		message += "\t at " + std::string(functionName) + "\n"
			+ "\t at line " + std::to_string(line) + "\n"
			+ "\t at file " + std::string(file);

		Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, message);

		throw std::runtime_error(message);
	}
}

void OSK::RuntimeCheckIsTrueFunction(bool condition, const std::string& msg, const std::string& functionName, uint32_t line, const std::string& file) {
	if (!condition) {
		std::string message = "Assertion failed: " + msg + "\n";
		message += "\t at " + std::string(functionName) + "\n"
			+ "\t at line " + std::to_string(line) + "\n"
			+ "\t at file " + std::string(file);

		Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, message);

		throw std::runtime_error(message);
	}
}

void OSK::RuntimeAssertIsFalseFunction(bool condition, const std::string& msg, const std::string& functionName, uint32_t line, const std::string& file) {
	if (condition) {
		std::string message = "Check failed: " + msg + "\n";
		message += "\t at " + std::string(functionName) + "\n"
			+ "\t at line " + std::to_string(line) + "\n"
			+ "\t at file " + std::string(file);

		Logger::Log(OSK::LogMessageLevels::WARNING, message);
	}
}

void OSK::RuntimeCheckIsFalseFunction(bool condition, const std::string& msg, const std::string& functionName, uint32_t line, const std::string& file) {
	std::string message = "Check failed: " + msg + "\n";
	message += "\t at " + std::string(functionName) + "\n"
		+ "\t at line " + std::to_string(line) + "\n"
		+ "\t at file " + std::string(file);

	Logger::Log(OSK::LogMessageLevels::WARNING, message);
}
