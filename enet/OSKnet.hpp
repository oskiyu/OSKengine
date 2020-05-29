#pragma once

#include <enet/enet.h>

namespace OSK::NET {

	inline void GlobalInit() {
		enet_initialize();
	}

	inline void GlobalShutdown() {
		enet_deinitialize();
	}

}