#include "ExitCommand.h"

#include "Game.h"

#include <span>
#include <string>

using namespace OSK;


ExitCommand::ExitCommand(IGame* game) : m_game(game) {
	SetCommandWords({ "exit" });
}

void ExitCommand::Execute(UI::Console* console, std::span<const std::string> params) const {
	if (m_game) {
		m_game->Exit();
	}
}
