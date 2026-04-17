#pragma once

#include "ApiCall.h"

#include <format>
#include <string>
#include <mutex>


#undef ERROR

namespace OSK::IO {

	/// @brief Nivel de importancia del mensaje
	/// de un ILogger.
	enum class LogLevel {

		/// @brief Sin ningún nivel establecido.
		NO,

		/// @brief Mensaje informativo.
		INFO,

		/// @brief Mensaje de depuración.
		L_DEBUG,

		/// @brief Mensaje de advertencia importante.
		WARNING, 

		/// @brief Mensaje de error irrecuperable.
		L_ERROR

	};


	/// @brief Clase que permite enviar mensajes por consola, o por
	/// donde se defina su salida.
	/// 
	/// También guarda el resultado del log.
	/// 
	/// Puede usarse el logger del motor desde Engine::GetLogger().
	class OSKAPI_CALL ILogger {

	public:

		/// @brief Cierra el logger, guardando el archivo.
		virtual ~ILogger() = default;


		/// @brief Inicializa el logger.
		/// @param path Ruta donde se guardará el archivo de log.
		virtual void Start(const std::string& path) = 0;

		/// @brief Manda un mensaje por el logger.
		/// @param level Nivel del mensaje.
		/// @param args Mensaje a mostrar.
		/// 
		/// @pre El logger debe haber sido inicializado con Start().
		/// @throws LoggerNotInitializedException si se incumple la precondición.
		/// 
		/// @threadsafe
		template <typename ...T>
		void Log(LogLevel level, T... args)
		{
			_Log(level, Append(args...));
		}
		
		/// @brief Manda un mensaje de depuración.
		/// @param msg Mensaje a mostrar.
		/// 
		/// @pre El logger debe haber sido inicializado con Start().
		/// @throws LoggerNotInitializedException si se incumple la precondición.
		/// 
		/// @threadsafe
		virtual void DebugLog(const std::string& msg) = 0;

		/// @brief Manda un mensaje de información.
		/// @param msg Mensaje a mostrar.
		/// 
		/// @pre El logger debe haber sido inicializado con Start().
		/// @throws LoggerNotInitializedException si se incumple la precondición.
		/// 
		/// @threadsafe
		virtual void InfoLog(const std::string& msg) = 0;

		/// @brief Guarda el archivo del log, en la ruta especificada
		/// al inicializarlo.
		/// @throws LoggerNotInitializedException si se incumple la precondición.
		virtual void Save() = 0;

		/// @brief Guarda el archivo del log.
		/// @param path Ruta donde guardar el archivo.
		/// @throws LoggerNotInitializedException si se incumple la precondición.
		virtual void Save(const std::string& path) = 0;

		/// @brief Limpia los contenidos del archivo del log.
		/// @throws LoggerNotInitializedException si se incumple la precondición.
		virtual void Clear() = 0;

		/// @brief Cierra el logger, guardando el archivo.
		/// @throws LoggerNotInitializedException si se incumple la precondición.
		virtual void Close() = 0;

protected:

		/// @brief Manda un mensaje por el logger.
		/// @param level Nivel del mensaje.
		/// @param msg Mensaje a mostrar.
		/// 
		/// @pre El logger debe haber sido inicializado con Start().
		/// @throws LoggerNotInitializedException si se incumple la precondición.
		/// 
		/// @threadsafe
		virtual void _Log(LogLevel level, const std::string& msg) = 0;

private:

		template <typename T, typename ...TArgs>
		std::string Append(const T& arg1, TArgs... args)
		{
			return std::format("{} {}", arg1, Append(args...));
		}

		template <typename T>
		std::string Append(const T& arg)
		{
			return std::format("{}", arg);
		}

	};


	class OSKAPI_CALL Logger : public ILogger {

	public:

		~Logger() override;

		void Start(const std::string& path);

		
		void _Log(LogLevel level, const std::string& msg);		
		void DebugLog(const std::string& msg);
		void InfoLog(const std::string& msg);

		void Save();
		void Save(const std::string& path);

		void Clear();
		void Close();

	private:

		// Contenido del archivo que se va a escribir.
		std::string m_output;
		std::string m_path;

		bool m_hasBeenStarted = false;

		std::mutex m_mutex;

	};

}
