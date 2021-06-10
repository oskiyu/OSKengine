#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif

#ifndef OSK_RELEASE_DLL
//mainCRTStartup
/**/
#include "VulkanRenderer.h"
#include "Camera2D.h"
#include "Sprite.h"
#include "ToString.h"
#include "Log.h"

#include "PhysicsSystem.h"

#include "Profiler.h"

#include "SAT_Collider.h"
#include "RayCast.h"

#include "ECS.h"
#include "AudioAPI.h"
#include "GameObject.h"
#include "InputSystem.h"

#include "Scanner.h"

#include "Game1.hpp"

int main() {

	OSK::Logger::Start();

	try {
		Game1 game;
		game.Run();
	}
	catch (std::runtime_error& e) {
		OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, e.what());
	}

	OSK::Logger::Close();

	return 0;
}

#endif
