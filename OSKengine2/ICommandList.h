#pragma once

#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Una lista de comandos contiene una serie de comandos que serán
	/// enviados a la GPU para su ejecución.
	/// La lista es creada por una pool de comandos, y se introduce
	/// en una cola de comandos para su ejecución.
	/// </summary>
	class OSKAPI_CALL ICommandList {

	public:

		~ICommandList() = default;

	};

}
