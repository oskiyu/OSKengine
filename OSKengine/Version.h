#pragma once

namespace OSK {

	//Representa una versi�n.
	struct OSKAPI_CALL Version {
		//Versi�n mayor.
		unsigned int Mayor;
		//Versi�n menor.
		unsigned int Menor;
		//Parche.
		unsigned int Parche;
	};

}
