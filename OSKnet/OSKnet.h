#pragma once

#include <enet/enet.h>

#include "Macros.h"

namespace OSK::NET {

	//Inicializa OSK::NET.
	//Necesario antes de hacer usar OSK::NET.
	bool OSKAPI_CALL GlobalInit();


	//Cierra OSK::NET.
	//Necesario antes de salir del programa.
	void OSKAPI_CALL GlobalShutdown();

}