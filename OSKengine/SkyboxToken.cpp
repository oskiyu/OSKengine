#include "SkyboxToken.h"

using namespace OSK::SceneSystem::Loader;

SkyboxToken::SkyboxToken() {
	tokenType = TokenType::SKYBOX;
}

void SkyboxToken::ProcessData(Scanner* sc) {
	path = sc->ProcessText();
}

std::string SkyboxToken::GetPath() const {
	return path;
}
