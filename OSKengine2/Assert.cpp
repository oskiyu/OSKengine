#include "Assert.h"

#include "OSKengine.h"
#include "Logger.h"

#include <stdexcept>

#ifdef _WIN64
#include <Windows.h>
#include "WindowsUtils.h"
#endif

using namespace OSK;

void OSK::RuntimeAssertIsTrueFunction(bool condition, const std::string& msg, const std::string& functionName, uint32_t line, const std::string& file) {
	if (!condition) {
		std::string message = "Assertion failed: " + msg + "\n";
		message += "\t at " + std::string(functionName) + "\n"
			+ "\t at line " + std::to_string(line) + "\n"
			+ "\t at file " + std::string(file);

		Engine::GetLogger()->Log(OSK::IO::LogLevel::L_ERROR, message);
		Engine::GetLogger()->Save();

#ifdef _WIN64
		auto widestring = StringToWideString(message);
		MessageBox(NULL, widestring.c_str(), NULL, MB_OK | MB_ICONERROR);
#endif // _WIN64

		Engine::GetWindow()->Close();

		throw std::runtime_error(message);
	}
}

void OSK::RuntimeCheckIsTrueFunction(bool condition, const std::string& msg, const std::string& functionName, uint32_t line, const std::string& file) {
	if (!condition) {
		std::string message = "Check failed: " + msg + "\n";
		message += "\t at " + std::string(functionName) + "\n"
			+ "\t at line " + std::to_string(line) + "\n"
			+ "\t at file " + std::string(file);

		Engine::GetLogger()->Log(OSK::IO::LogLevel::L_ERROR, message);
	}
}

void OSK::RuntimeAssertIsFalseFunction(bool condition, const std::string& msg, const std::string& functionName, uint32_t line, const std::string& file) {
	if (condition) {
		std::string message = "Assertion failed: " + msg + "\n";
		message += "\t at " + std::string(functionName) + "\n"
			+ "\t at line " + std::to_string(line) + "\n"
			+ "\t at file " + std::string(file);

		Engine::GetLogger()->Log(OSK::IO::LogLevel::WARNING, message);
		Engine::GetLogger()->Save();
	}
}

void OSK::RuntimeCheckIsFalseFunction(bool condition, const std::string& msg, const std::string& functionName, uint32_t line, const std::string& file) {
	std::string message = "Check failed: " + msg + "\n";
	message += "\t at " + std::string(functionName) + "\n"
		+ "\t at line " + std::to_string(line) + "\n"
		+ "\t at file " + std::string(file);

	Engine::GetLogger()->Log(OSK::IO::LogLevel::WARNING, message);
}
