#pragma once

#include "DynamicArray.hpp"

#include <string>
#include <xstring>

namespace OSK {

	/// @brief 
	/// @param string 
	/// @param delimiter 
	/// @return 
	DynamicArray<std::string> Tokenize(std::string_view string, char delimiter);

	/// @brief 
	/// @param string 
	/// @return 
	std::wstring StringToWideString(std::string_view string);

}
