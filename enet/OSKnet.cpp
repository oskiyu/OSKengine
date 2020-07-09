#include "OSKnet.h"

namespace OSK::NET {

	bool GlobalInit() {
		return enet_initialize();
	}


	void GlobalShutdown() {
		return enet_deinitialize();
	}

}