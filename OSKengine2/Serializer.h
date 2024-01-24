// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "DataNode.h"

#include <json.hpp>

namespace OSK::PERSISTENCE {

	// --- JSON: Scene Editor --- //

	template <typename T>
	nlohmann::json SerializeJson(const T& data) = delete;


	template <typename T>
	T DeserializeJson(const nlohmann::json& data) = delete;

}
