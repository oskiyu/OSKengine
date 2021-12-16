#include <string>
#include <stdexcept>
#include <iostream>

export module OSKengine.Logger;

export namespace OSK {

	enum class LogLevel {
		INFO,
		DEBUG,
		WARNING,
		ERROR,
		COMMAND,
		NO
	};
	
	class Logger {

	public:

		~Logger() {
			Close();
		}

		void Start(const std::string& targetPath) {
			path = targetPath;
			hasBeenStarted = true;
		}

		void Log(LogLevel level, const std::string& msg) {
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

			case LogLevel::ERROR:
				start = "ERROR: ";
				break;

			case LogLevel::DEBUG:
				start = "DEBUG: ";
				break;
			}

#ifdef OSK_DEBUG
			std::cout << start << msg << std::endl;
#else
			output.append(start);
			output.append(msg);

			output.append("\n");
#endif
		}

		void DebugLog(const std::string& message) {
			Log(LogLevel::DEBUG, message);
		}
		void InfoLog(const std::string& message) {
			Log(LogLevel::INFO, message);
		}

		void Save() {
			Save(path);
		}
		void Save(const std::string& filename) {
			if (!hasBeenStarted)
				throw std::runtime_error("OSK::Logger no ha sido inicializado. Usa Logger::Start() antes.");

#ifdef OSK_RELEASE
			std::ofstream outputLog(path + filename);
			if (outputLog.is_open()) {
				outputLog << output << std::endl;
			}
			outputLog.close();
#endif // OSK_RELEASE
		}

		void Clear() {
			if (!hasBeenStarted)
				throw std::runtime_error("OSK::Logger no ha sido inicializado. Usa Logger::Start() antes.");

			output.clear();
		}

		void Close() {
			if (!hasBeenStarted)
				throw std::runtime_error("OSK::Logger no ha sido inicializado. Usa Logger::Start() antes.");

			Save();
			Clear();
		}

	private:

		std::string path = "";
		std::string output = "";
		bool hasBeenStarted = false;

	};

	Logger* GetLogger() {
		static Logger log;

		return &log;
	}

}
