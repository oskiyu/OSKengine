#include "PropertiesFile.h"

#include "FileIO.h"

using namespace OSK;

void PropertiesFile::ReadFile(const std::string& path) {
	auto code = FileIO::ReadFromFile(path);

	size_t pos = 0;
	std::string line;
	while ((pos = code.find("\n")) != std::string::npos) {
		line = code.substr(0, pos);

		size_t equalsPos = line.find("=");
		std::string name = line.substr(0, equalsPos);
		std::string value = line.substr(equalsPos + 1, line.length());

		values[name] = value;

		code.erase(0, pos + std::string("\n").length());
	}
}

int PropertiesFile::GetInt(const std::string& name) {
	return std::stoi(values.at(name));
}

float PropertiesFile::GetFloat(const std::string& name) {
	return std::stof(values.at(name));
}

std::string PropertiesFile::GetString(const std::string& name) {
	return values.at(name);
}

bool PropertiesFile::GetBoolean(const std::string& name) {
	auto value = values.at(name);

	if (value == "true")
		return true;

	return false;
}
