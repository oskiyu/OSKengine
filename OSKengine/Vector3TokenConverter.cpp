#include "Vector3TokenConverter.h"

using namespace OSK;
using namespace OSK::SceneSystem::Loader;

void Vector3TokenConverter::Process(const std::string& sdata) {
	data = sdata;
	charPos = 0;

	ConsumeSpace();
	x = GetNextNumber();
	ConsumeSpace();
	charPos++;
	ConsumeSpace();
	y = GetNextNumber();
	ConsumeSpace();
	charPos++;
	ConsumeSpace();
	z = GetNextNumber();
}

Vector3f Vector3TokenConverter::GetVector3() const {
	return { x, y, z };
}

float Vector3TokenConverter::GetNextNumber() {
	std::string thisData = "";

	while (Scanner::IsNumber(NextChar())) {
		thisData += NextChar();
		charPos++;
	}

	return std::stof(thisData);
}

void Vector3TokenConverter::ConsumeSpace() {
	while (Scanner::IsSpace(NextChar()))
		charPos++;
}

char Vector3TokenConverter::NextChar() const {
	return data[charPos];
}
