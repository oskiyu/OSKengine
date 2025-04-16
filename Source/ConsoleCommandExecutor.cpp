#include "ConsoleCommandExecutor.h"

#include <string_view>

using namespace OSK;


void ConsoleCommandExecutor::RegisterCommand(UniquePtr<IConsoleCommand>&& command) {
	m_registeredCommands.Insert(std::move(command));
}

void ConsoleCommandExecutor::CheckAndExecute(UI::Console* console, std::string_view text) const {
	const DynamicArray<std::string> parts = SplitMessage(text);

	for (const auto& command : m_registeredCommands) {
		if (command->IsCompatibleWith(parts.GetFullSpan())) {
			command->Execute(
				console,
				parts.GetSpan(
					command->GetCommandWords().size(),
					parts.GetSize() - command->GetCommandWords().size()));

			return;
		}
	}
}

DynamicArray<std::string> ConsoleCommandExecutor::SplitMessage(std::string_view message) const {
	DynamicArray<std::string> parts;

	std::string temp = "";
	for (const char c : message) {

		if (c == ' ' || c == '\t') {
			if (!temp.empty()) {
				parts.Insert(temp);
			}

			temp = "";
			continue;
		}

		temp.push_back(c);
	}

	if (!temp.empty()) {
		parts.Insert(temp);
	}

	return parts;
}
