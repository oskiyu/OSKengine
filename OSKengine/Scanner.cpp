#include "Scanner.h"

#include "VersionToken.h"
#include "SkyboxToken.h"
#include "TerrainToken.h"
#include "PlaceToken.h"

using namespace OSK::SceneSystem::Loader;

Scanner::~Scanner() {
	Clear();
}

std::vector<IToken*>& Scanner::GetTokens(const std::string& code) {
	sourceCode = code;

	while (currentChar < code.length())
		AddNextToken();

	return tokens;
}

std::string Scanner::ProcessText() {
	std::string output = "";

	while (IsSpace(PeekNextChar()))
		currentChar++;

	while (PeekNextChar() != ' ') {
		if (PeekNextChar() == '\0')
			return output;

		if (PeekNextChar() == '\n') {
			currentChar++;

			return output;
		}

		output += PeekNextChar();
		currentChar++;
	}

	return output;
}

int Scanner::ProcessInteger() {
	std::string output = "";

	while (IsSpace(PeekNextChar()))
		currentChar++;

	while (IsNumber(PeekNextChar())) {
		output += PeekNextChar();
		currentChar++;
	}

	return std::stoi(output);
}

float Scanner::ProcessFloat() {
	std::string output = "";

	while (IsSpace(PeekNextChar()))
		currentChar++;

	while (IsNumber(PeekNextChar())) {
		output += PeekNextChar();
		currentChar++;
	}

	return std::stof(output);
}

std::string Scanner::ProcessDataToken() {
	std::string data = "";

	while (PeekNextChar() != '(')
		currentChar++;

	currentChar++;

	while (PeekNextChar() != ')') {
		data += PeekNextChar();
		currentChar++;
	}
	currentChar++;

	return data;
}

bool Scanner::IsNumber(char c) {
	switch (c) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '.':
	case '-':
		return true;
	}

	return false;
}

bool Scanner::IsSpace(char c) {
	switch (c) {
		case ' ':
		case '\n':
		case '\t':
			return true;
	}

	return false;
}

char Scanner::PeekNextChar() const {
	if (currentChar >= sourceCode.length())
		return '\0';

	return sourceCode[currentChar];
}

void Scanner::AddNextToken() {
	std::string text = ProcessText();

	if (text == "place") {
		PlaceToken* token = new PlaceToken;
		token->ProcessData(this);

		tokens.push_back(token);
	}

	if (text == "version") {
		VersionToken* version = new VersionToken;
		version->ProcessData(this);

		tokens.push_back(version);
	}
	if (text == "skybox") {
		SkyboxToken* token = new SkyboxToken;
		token->ProcessData(this);

		tokens.push_back(token);
	}
	if (text == "terrain") {
		TerrainToken* token = new TerrainToken;
		token->ProcessData(this);

		tokens.push_back(token);
	}
}

void Scanner::Clear() {
	for (auto i : tokens)
		delete i;
	tokens.clear();

	currentChar = 0;
	linea = 0;
}
