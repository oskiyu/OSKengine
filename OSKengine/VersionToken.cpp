#include "VersionToken.h"

using namespace OSK::SceneSystem::Loader;

VersionToken::VersionToken() {
	tokenType = TokenType::VERSION;
}

void VersionToken::ProcessData(Scanner* sc) {
	version = sc->ProcessInteger();
}

int VersionToken::GetVersion() const {
	return version;
}