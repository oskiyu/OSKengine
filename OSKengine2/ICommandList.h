#pragma once

#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Una lista de comandos contiene una serie de comandos que ser�n
	/// enviados a la GPU para su ejecuci�n.
	/// La lista es creada por una pool de comandos, y se introduce
	/// en una cola de comandos para su ejecuci�n.
	/// </summary>
	class OSKAPI_CALL ICommandList {

	public:

		~ICommandList() = default;

		template <typename T> T* As() const {
			return (T*)this;
		}

		virtual void Reset() = 0;
		virtual void Start() = 0;
		virtual void Close() = 0;

	};

}
