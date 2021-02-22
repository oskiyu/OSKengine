#pragma once

#define OSK_USE_INFO_LOGS //OSK::Log() se usará también con mensajes de información, no solo con errores. 

namespace OSK {

	constexpr auto ENGINE_NAME = "OSKengine";

	constexpr auto ENGINE_VERSION_STAGE = "Alpha";

	constexpr auto ENGINE_VERSION_STAGE_NUMERIC = 0;

	constexpr auto ENGINE_VERSION_NUMERIC = 17;

	constexpr auto ENGINE_VERSION_MINOR = "17*";

	constexpr auto ENGINE_VERSION_ALPHANUMERIC = "Alpha 17*";

	constexpr auto ENGINE_VERSION_BUILD = "2021.01.29a";

	constexpr auto ENGINE_VERSION_BUILD_NUMERIC = 20210129;
	
	constexpr auto ENGINE_VERSION = "0.17* build 2021.01.29a";

	constexpr auto ENGINE_VERSION_TOTAL = 17;

}