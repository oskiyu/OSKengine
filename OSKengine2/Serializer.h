// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "DataNode.h"

namespace OSK::PERSISTENCE {

	template <typename T>
	DataNode Serialize(const T& data) {
		static_assert(false);
	};

	template <typename T>
	T Deserialize(const DataNode& dataNode) {
		static_assert(false);
	};

}
