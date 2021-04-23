#pragma once

#define OSK_USE_INFO_LOGS //OSK::Log() se usará también con mensajes de información, no solo con errores. 

#include <string>

namespace OSK {

	/// <summary>
	/// Nombre de OSKengine.
	/// </summary>
	extern std::string ENGINE_NAME;

	/// <summary>
	/// Versión mayor de OSKengine.
	/// </summary>
	extern std::string ENGINE_VERSION_STAGE;

	/// <summary>
	/// Versión mayor de OSKengine, en número.
	/// </summary>
	extern int ENGINE_VERSION_STAGE_NUMERIC;

	/// <summary>
	/// Versión de OSKengine, en número.
	/// </summary>
	extern int ENGINE_VERSION_NUMERIC;

	/// <summary>
	/// Versión de OSKengine.
	/// </summary>
	extern std::string ENGINE_VERSION_MINOR;

	/// <summary>
	/// Versión total.
	/// </summary>
	extern std::string ENGINE_VERSION_ALPHANUMERIC;

	/// <summary>
	/// Build de OSKengine.
	/// YY.MM.DDn
	/// </summary>
	extern std::string ENGINE_VERSION_BUILD;

	/// <summary>
	/// Build de OSKengine.
	/// YYMMDDn
	/// </summary>
	extern int ENGINE_VERSION_BUILD_NUMERIC;
	
	/// <summary>
	/// Versión total.
	/// </summary>
	extern std::string ENGINE_VERSION;

	/// <summary>
	/// Número de versión.
	/// </summary>
	extern int ENGINE_VERSION_TOTAL;

}