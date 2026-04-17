#pragma once

#include "Platforms.h"
#include "DynamicArray.hpp"

#include <string>

#ifdef OSK_WINDOWS
#include <xstring>
#endif

namespace OSK {

	/// @brief 
	/// @param string 
	/// @param delimiter 
	/// @return 
	DynamicArray<std::string> Tokenize(std::string_view string, char delimiter);

#ifdef OSK_WINDOWS
	/// @brief 
	/// @param string 
	/// @return 
	std::wstring StringToWideString(std::string_view string);
#endif

}
