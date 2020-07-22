#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include <fstream>
#include <vector>

namespace OSK {

	OSK_INFO_STATIC
	//Clase que actúa de interfaz para la lectura y escritura de atchivos.
	class OSKAPI_CALL FileIO {
	
	public:

		//Escribe un archivo de texto.
		//	<path>: ruta del archivo.
		//	<text>: texto que se va a escribir.
		static void WriteFile(const std::string& path, const std::string& text);

		//Lee un archivo de texto.
		//	<path>: ruta del archivo.
		static std::string ReadFromFile(const std::string& path);

		//Lee un archivo binario.
		//	<filename>: ruta del archivo.
		static std::vector<char> ReadBinaryFromFile(const std::string& filename);

	};

}