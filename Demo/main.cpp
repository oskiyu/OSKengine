#include "Game.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/Window.h>

int main() {
#define DEBUG
#ifdef DEBUG
	Game demo{};
	demo._Run();

	return 0;
#else
	try {
		Game demo{};
		demo._Run();

		return 0;
	}
	catch (const OSK::EngineException& e) {
		const std::string message = std::format("Excepción producida: {}", e.what());
		if (OSK::Engine::GetLogger()) OSK::Engine::GetLogger()->Log(OSK::IO::LogLevel::L_ERROR, message);
		OSK::IO::Window::ShowMessageBox(message);

		return -1;
	}
#endif // DEBUG
}
