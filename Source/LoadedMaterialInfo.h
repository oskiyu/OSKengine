#pragma once

#include <string>
#include <optional>
#include <unordered_map>

#include "NumericTypes.h"

namespace OSK::GRAPHICS {

	struct LoadedMaterialInfoV1 {

		std::string name;

		struct Config {
			USize32 resourceMaxCount = 0;
			bool usesUnspecifiedSizedArrays = false;
		} config{};

		std::optional<std::string> vertexShaderPath;
		std::optional<std::string> fragmentShaderPath;

		std::optional<std::string> tesselationControlShaderPath;
		std::optional<std::string> tesselationEvaluationShaderPath;

		std::optional<std::string> computeShaderPath;

		std::optional<std::string> rayGenShaderPath;
		std::optional<std::string> rayMissShaderPath;
		std::optional<std::string> rayClosestHitShaderPath;

		std::optional<std::string> meshAmplificationShaderPath;
		std::optional<std::string> meshShaderPath;

		std::unordered_map<OSK::UIndex32, std::string> slotNames;

	};

}
