#pragma once

#include "ApiCall.h"
#include "ConsoleCommand.h"

#include <string>
#include <span>

namespace OSK {

	class IGame;

	class OSKAPI_CALL ExitCommand : public IConsoleCommand {

	public:

		explicit ExitCommand(IGame* game);
		void Execute(UI::Console* console, std::span<const std::string> params) const override;

	private:

		IGame* m_game = nullptr;

	};

}
