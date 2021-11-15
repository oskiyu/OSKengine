#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"

namespace OSK {

	class ICommandPool;

	/// <summary>
	/// Interfaz de una tarjeta gráfica.
	/// Esta interfaz no implementa ningún código, se debe usar una instancia de
	/// GpuVulkan o GpuDx12.
	/// </summary>
	class OSKAPI_CALL IGpu {

	public:

		virtual ~IGpu() = default;

		virtual OwnedPtr<ICommandPool> CreateCommandPool() = 0;

		virtual void Close() = 0;

		/// <summary>
		/// Castea la clase al tipo dado.
		/// Este tipo debe ser una implementación de esta interfaz.
		/// </summary>
		template <typename T> T* As() const {
			return (T*)this;
		}

	protected:

		IGpu() { }

	private:

	};

}
