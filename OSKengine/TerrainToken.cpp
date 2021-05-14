#include "TerrainToken.h"

using namespace OSK::SceneSystem::Loader;

TerrainToken::TerrainToken() {
	tokenType = TokenType::TERRAIN;
}

void TerrainToken::ProcessData(Scanner* sc) {
	path = sc->ProcessText();
}

std::string TerrainToken::GetPath() const {
	return path;
}
