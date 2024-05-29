#include "EditorCommands.h"

#include "UiConsole.h"
#include "EditorUi.h"

#include <span>
#include <string>

using namespace OSK;


EditorShowCommand::EditorShowCommand() {
	SetCommandWords({ "editor", "show" });
}

void EditorShowCommand::Execute(UI::Console* console, std::span<const std::string> params) const {
	if (editor) {
		editor->SetVisible();
	}
}


EditorHideCommand::EditorHideCommand() {
	SetCommandWords({ "editor", "hide" });
}

void EditorHideCommand::Execute(UI::Console* console, std::span<const std::string> params) const {
	if (editor) {
		editor->SetInvisible();
	}
}
