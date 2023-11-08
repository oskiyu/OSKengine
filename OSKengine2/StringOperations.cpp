#include "StringOperations.h"

#include <Windows.h>

OSK::DynamicArray<std::string> OSK::Tokenize(std::string_view string, char delimiter) {
	DynamicArray<std::string> output;

	size_t currentOffset = 0;
	size_t nextOffset = 0; 

	while (true) {
		nextOffset = string.find(delimiter, currentOffset);

		if (nextOffset == std::string_view::npos)
			break;

		output.Insert(static_cast<std::string>(string.substr(currentOffset, nextOffset - currentOffset)));
	}

	return output;
}

std::wstring OSK::StringToWideString(std::string_view string) {
	const int stringLength = static_cast<int>(string.length()) + 1;

	const int wideStringLength = MultiByteToWideChar(CP_ACP, 0, string.data(), stringLength, nullptr, 0);

	wchar_t* buf = new wchar_t[wideStringLength];
	MultiByteToWideChar(CP_ACP, 0, string.data(), stringLength, buf, wideStringLength);
	
	std::wstring r(buf);
	delete[] buf;

	return r;
}
