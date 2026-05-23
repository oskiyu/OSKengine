#include "EngineException.h"
#include "Game1.hpp"
#include "Logger.h"

#include "Platforms.h"

const char* CmdToStr(int32_t cmd) {
    switch (cmd) {
    case  0: return "APP_CMD_INPUT_CHANGED";
    case  1: return "APP_CMD_INIT_WINDOW"; // a new ANativeWindow is ready for use
    case  2: return "APP_CMD_TERM_WINDOW"; // the existing ANativeWindow needs to be terminated
    case  3: return "APP_CMD_WINDOW_RESIZED";
    case  4: return "APP_CMD_WINDOW_REDRAW_NEEDED";
    case  5: return "APP_CMD_CONTENT_RECT_CHANGED";
    case  6: return "APP_CMD_GAINED_FOCUS";
    case  7: return "APP_CMD_LOST_FOCUS";
    case  8: return "APP_CMD_CONFIG_CHANGED";
    case  9: return "APP_CMD_LOW_MEMORY";
    case 10: return "APP_CMD_START";
    case 11: return "APP_CMD_RESUME";
	case 12: return "APP_CMD_SAVE_STATE";
	case 13: return "APP_CMD_PAUSE";
	case 14: return "APP_CMD_STOP";
	case 15: return "APP_CMD_DESTROY";
	case 16: return "APP_CMD_PAUSE";

    default: return "";
    };
};


#ifdef OSK_ANDROID
#include <android_native_app_glue.h>
#include <android/log.h>
#include <chrono>
#include <thread>

UniquePtr<GameMin> game;


void HandleAppCommand(android_app* app, int32_t cmd) {
	__android_log_print(ANDROID_LOG_ERROR, "OSKENGINE", "%s %s", "APP_CMD: ", CmdToStr(cmd));

	switch (cmd) {

	case APP_CMD_INIT_WINDOW:
		__android_log_print(ANDROID_LOG_ERROR, "OSKENGINE", "%s", "RUN");
		app->onAppCmd = IGame::AndroidHandleAppCommand;
		app->userData = game.GetPointer();
		game->_Run(app);
		break;

	};
}

extern "C" {

	void android_main(struct android_app* app) {
		app_dummy();

		app->onAppCmd = HandleAppCommand;
		// ALooper_prepare(ALOOPER_PREPARE_ALLOW_ALL_HOOKS);

		__android_log_print(ANDROID_LOG_ERROR, "OSKENGINE", "%s", "Inicialización de OSKengine");

		try {
			game = MakeUnique<GameMin>();
			__android_log_print(ANDROID_LOG_ERROR, "OSKENGINE", "%s", "Creado GameMin");

			while (true) {
				struct android_poll_source* source;

				int events = 0;
				int event = ALooper_pollOnce(0, nullptr, &events, (void**)&source);

				if (event > 0) {
					if (source != NULL) {
						source->process(app, source);
					}

					if (app->destroyRequested != 0) {
						return;
					}
				}
			}
		}
		catch (const OSK::EngineException& e) {
			Engine::GetLogger()->Log(IO::LogLevel::L_ERROR, e.what());
		}
	}

}

#else

int main() {

#ifdef OSK_RELEASE
	try {
		auto game = GameMin();
		game._Run();
	}
	catch (const OSK::EngineException& e) {
		const std::string message = std::format("Excepción producida: {}", e.what());
		if (OSK::Engine::GetLogger()) OSK::Engine::GetLogger()->Log(OSK::IO::LogLevel::L_ERROR, message);
		OSK::IO::Window::ShowMessageBox(message);

		throw e;
	}
#else
	GameMin game;
	try {
		game._Run();
	}
	catch (const OSK::EngineException& e) {
		Engine::GetLogger()->Log(IO::LogLevel::L_ERROR, e.what());
	}
#endif // OSK_RELEASE

	return 0;
}

#endif // OSK_ANDROID
