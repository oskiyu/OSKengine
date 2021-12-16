#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include <type_traits>

namespace OSK {

	class ICommandList;
	class IGpu;

	/// <summary>
	/// Una pool de comandos se encarga de crear una serie de listas de comandos.
	/// </summary>
	class OSKAPI_CALL ICommandPool {

	public:

		virtual ~ICommandPool() = default;

		/// <summary>
		/// Crea una lista de comandos.
		/// </summary>
		/// <param name="device">GPU donde se alamcenarán las listas.</param>
		virtual OwnedPtr<ICommandList> CreateCommandList(const IGpu& device) = 0;

		template <typename T> T* As() const requires std::is_base_of_v<ICommandPool, T> {
			return (T*)this;
		}

	};

}
