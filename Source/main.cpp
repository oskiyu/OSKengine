#include "Game1.hpp"

#ifdef OSK_DEVELOPMENT
int main() {


#ifdef OSK_RELEASE
	try {
		Game1 game;
		game._Run();
	}
	catch (const OSK::EngineException& e) {
		const std::string message = std::format("Excepción producida: {}", e.what());
		if (OSK::Engine::GetLogger()) OSK::Engine::GetLogger()->Log(OSK::IO::LogLevel::L_ERROR, message);
		OSK::IO::Window::ShowMessageBox(message);

		throw e;
	}
#else
	Game1 game;
	game._Run();
#endif // OSK_RELEASE

	return 0;
}

#endif // OSK_DEVELOPMENT
