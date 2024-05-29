#pragma once

#include "ApiCall.h"
#include "ConsoleCommand.h"

#include <string>
#include <span>

namespace OSK {

	namespace Editor::UI {
		class EditorUi;
	}


	class OSKAPI_CALL EditorShowCommand : public IConsoleCommand {

	public:

		explicit EditorShowCommand();
		void Execute(UI::Console* console, std::span<const std::string> params) const override;

		Editor::UI::EditorUi* editor = nullptr;

	};

	class OSKAPI_CALL EditorHideCommand : public IConsoleCommand {

	public:

		explicit EditorHideCommand();
		void Execute(UI::Console* console, std::span<const std::string> params) const override;

		Editor::UI::EditorUi* editor = nullptr;

	};

}
