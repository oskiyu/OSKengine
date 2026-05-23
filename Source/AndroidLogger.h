#pragma once

#include "Platforms.h"
#ifdef OSK_ANDROID

#include "Logger.h"

namespace OSK::IO {

	/// @brief Logger específico para Android.
	///
	/// Muestra los mensajes con el tag "OSKENGINE".
	/// 
	/// @warning No escribe el log en almacenamiento,
	/// aunque se llame a `Save`.
	class OSKAPI_CALL AndroidLogger : public ILogger {

	public:

		~AndroidLogger() override;

		void Start(const std::string& path) override;


		void _Log(LogLevel level, const std::string& msg) override;
		void DebugLog(const std::string& msg) override;
		void InfoLog(const std::string& msg) override;

		void Save() override;
		void Save(const std::string& path) override;

		void Clear() override;
		void Close() override;

	private:

		std::string m_output;
		std::string m_path;

		bool m_hasBeenStarted = false;

		std::mutex m_mutex;

	};

}

#endif // OSK_ANDROID
