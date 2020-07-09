#pragma once

#include <enet/enet.h>

#include "Macros.h"

namespace OSK::NET {

	//Inicializa OSK::NET.
	//Necesario antes de hacer usar OSK::NET.
	OSKAPI_CALL bool GlobalInit();


	//Cierra OSK::NET.
	//Necesario antes de salir del programa.
	OSKAPI_CALL void GlobalShutdown();

}