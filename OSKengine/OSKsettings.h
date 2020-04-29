#pragma once

#define OSK_USE_INFO_LOGS //OSK::Log() se usará también con mensajes de información, no solo con errores. 

//#define OSK_ENGINE_LIB

namespace OSK {

	constexpr auto ENGINE_NAME = "OSKengine";

	constexpr auto ENGINE_VERSION_MAYOR_NUMERIC = 0;

	constexpr auto ENGINE_VERSION_MAYOR = "Alpha";

	constexpr auto ENGINE_VERSION_MINOR_NUMERIC = 15;

	constexpr auto ENGINE_VERSION_MINOR = "15";

	constexpr auto ENGINE_VERSION_ALPHANUMERIC = "Alpha 15";

	constexpr auto ENGINE_VERSION = "0.15";

	constexpr auto ENGINE_VERSION_TOTAL = 15;

}