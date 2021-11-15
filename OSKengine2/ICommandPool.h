#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"

namespace OSK {

	class ICommandList;
	class IGpu;

	/// <summary>
	/// Una pool de comandos se encarga de crear una serie de listas de comandos.
	/// </summary>
	class OSKAPI_CALL ICommandPool {

	public:

		~ICommandPool() = default;

		/// <summary>
		/// Crea una lista de comandos.
		/// </summary>
		/// <param name="device">GPU donde se alamcenarán las listas.</param>
		virtual OwnedPtr<ICommandList> CreateCommandList(const IGpu& device) = 0;

		template <typename T> T* As() const {
			return (T*)this;
		}

	};

}
