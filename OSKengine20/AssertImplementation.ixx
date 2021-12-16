#include <string>
#include <stdexcept>

export module OSKengine.Assert;

export import OSKengine.Logger;

export namespace OSK {

	void RuntimeAssertIsTrueFunction(bool condition, const std::string& msg, const std::string& functionName, uint32_t line, const std::string& file) {
		if (!condition) {
			std::string message = "Assertion failed: " + msg + "\n";
			message += "\t at " + std::string(functionName) + "\n"
				+ "\t at line " + std::to_string(line) + "\n"
				+ "\t at file " + std::string(file);

			GetLogger()->Log(LogLevel::ERROR, message);

			throw std::runtime_error(message);
		}
	}

	void RuntimeCheckIsTrueFunction(bool condition, const std::string& msg, const std::string& functionName, uint32_t line, const std::string& file) {
		if (!condition) {
			std::string message = "Assertion failed: " + msg + "\n";
			message += "\t at " + std::string(functionName) + "\n"
				+ "\t at line " + std::to_string(line) + "\n"
				+ "\t at file " + std::string(file);

			GetLogger()->Log(LogLevel::WARNING, message);

			throw std::runtime_error(message);
		}
	}

	void RuntimeAssertIsFalseFunction(bool condition, const std::string& msg, const std::string& functionName, uint32_t line, const std::string& file) {
		if (condition) {
			std::string message = "Check failed: " + msg + "\n";
			message += "\t at " + std::string(functionName) + "\n"
				+ "\t at line " + std::to_string(line) + "\n"
				+ "\t at file " + std::string(file);

			GetLogger()->Log(LogLevel::ERROR, message);
		}
	}

	void RuntimeCheckIsFalseFunction(bool condition, const std::string& msg, const std::string& functionName, uint32_t line, const std::string& file) {
		std::string message = "Check failed: " + msg + "\n";
		message += "\t at " + std::string(functionName) + "\n"
			+ "\t at line " + std::to_string(line) + "\n"
			+ "\t at file " + std::string(file);

		GetLogger()->Log(LogLevel::WARNING, message);
	}

}
