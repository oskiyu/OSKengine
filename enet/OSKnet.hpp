#pragma once

#include <enet/enet.h>

namespace OSK::NET {

	//Inicializa OSK::NET.
	//Necesario antes de hacer cualquier cosa con OSK::NET.
	inline void GlobalInit() {
		enet_initialize();
	}


	//Cierra OSK::NET.
	//Necesario antes de salir del programa.
	inline void GlobalShutdown() {
		enet_deinitialize();
	}

}