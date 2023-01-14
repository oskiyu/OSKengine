// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "Field.h"
#include "HashMap.hpp"

namespace OSK::PERSISTENCE {

	/// @brief 
	class Data {

	public:

		void SetField(const std::string& key, const IFieldWrapper& field) {
			fields.Insert(key, field);
		}

		const IFieldWrapper& GetField(const std::string& key) const {
			return fields.Get(key);
		}

	private:

		HashMap<std::string, IFieldWrapper> fields;

	};

}
