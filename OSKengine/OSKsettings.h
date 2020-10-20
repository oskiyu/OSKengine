#pragma once

#define OSK_USE_INFO_LOGS //OSK::Log() se usará también con mensajes de información, no solo con errores. 

namespace OSK {

	constexpr auto ENGINE_NAME = "OSKengine";

	constexpr auto ENGINE_VERSION_STAGE = "Alpha";

	constexpr auto ENGINE_VERSION_STAGE_NUMERIC = 0;

	constexpr auto ENGINE_VERSION_NUMERIC = 16;

	constexpr auto ENGINE_VERSION_MINOR = "16";

	constexpr auto ENGINE_VERSION_ALPHANUMERIC = "Alpha 16";

	constexpr auto ENGINE_VERSION_BUILD = "2020.10.20a";

	constexpr auto ENGINE_VERSION_BUILD_NUMERIC = 20201020;

	constexpr auto ENGINE_VERSION = "0.16 build 2020.10.20a";

	constexpr auto ENGINE_VERSION_TOTAL = 16;

}