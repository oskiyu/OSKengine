#pragma once

#define OSK_USE_INFO_LOGS //OSK::Log() se usar� tambi�n con mensajes de informaci�n, no solo con errores. 

#include <string>

namespace OSK {

	/// <summary>
	/// Nombre de OSKengine.
	/// </summary>
	extern std::string ENGINE_NAME;

	/// <summary>
	/// Versi�n mayor de OSKengine.
	/// </summary>
	extern std::string ENGINE_VERSION_STAGE;

	/// <summary>
	/// Versi�n mayor de OSKengine, en n�mero.
	/// </summary>
	extern int ENGINE_VERSION_STAGE_NUMERIC;

	/// <summary>
	/// Versi�n de OSKengine, en n�mero.
	/// </summary>
	extern int ENGINE_VERSION_NUMERIC;

	/// <summary>
	/// Versi�n de OSKengine.
	/// </summary>
	extern std::string ENGINE_VERSION_MINOR;

	/// <summary>
	/// Versi�n total.
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
	/// Versi�n total.
	/// </summary>
	extern std::string ENGINE_VERSION;

	/// <summary>
	/// N�mero de versi�n.
	/// </summary>
	extern int ENGINE_VERSION_TOTAL;

}