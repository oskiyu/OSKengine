#pragma once

#include "OSKmacros.h"

#include <unordered_map>
#include <string>

namespace OSK {

	class OSKAPI_CALL PropertiesFile {

	public:

		void ReadFile(const std::string& path);

		int GetInt(const std::string& name);
		float GetFloat(const std::string& name);
		std::string GetString(const std::string& name);
		bool GetBoolean(const std::string& name);

	private:

		std::unordered_map<std::string, std::string> values;

	};

}
