#include "ConsoleCommand.h"

#include "Assert.h"
#include "InvalidArgumentException.h"

using namespace OSK;


bool IConsoleCommand::IsCompatibleWith(std::span<const std::string> messageWords) const {
	if (m_commandText.GetSize() > messageWords.size()) {
		return false;
	}

	for (UIndex64 i = 0; i < messageWords.size(); i++) {
		const std::string_view originalWord = m_commandText[i];
		const std::string_view messageWord = m_commandText[i];

		if (originalWord != messageWord) {
			return false;
		}
	}

	return true;
}

std::span<const std::string> IConsoleCommand::GetCommandWords() const {
	return m_commandText.GetFullSpan();
}

void IConsoleCommand::SetCommandWords(const DynamicArray<std::string>& words) {
	OSK_ASSERT(
		!words.IsEmpty(),
		InvalidArgumentException("El comando necesita tener al menos una palabra."));

	m_commandText = words;
}

void IConsoleCommand::SetCommandWords(DynamicArray<std::string>&& words) {
	OSK_ASSERT(
		!words.IsEmpty(),
		InvalidArgumentException("El comando necesita tener al menos una palabra."));

	m_commandText = std::move(words);
}
