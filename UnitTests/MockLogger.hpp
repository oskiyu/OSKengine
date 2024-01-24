#include "../OSKengine2/Logger.h"

class MockLogger : public OSK::IO::ILogger {

	void Start(const std::string&) {
		(void)0;
	}

	void Log(OSK::IO::LogLevel, const std::string&) {
		(void)0;
	}

	void DebugLog(const std::string&) {
		(void)0;
	}

	void InfoLog(const std::string&) {
		(void)0;
	}

	void Save() {
		(void)0;
	}

	void Save(const std::string&) {
		(void)0;
	}

	void Clear() {
		(void)0;
	}

	void Close() {
		(void)0;
	}

};
