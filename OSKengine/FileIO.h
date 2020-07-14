#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include <fstream>
#include <vector>

namespace OSK {

	OSK_INFO_STATIC
	class OSKAPI_CALL FileIO {
	
	public:

		static void WriteFile(const std::string& path, const std::string& text);


		static std::string ReadFromFile(const std::string& path);


		static std::vector<char> ReadBinaryFromFile(const std::string& filename);

	};

}