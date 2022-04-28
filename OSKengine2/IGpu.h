#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include <type_traits>

namespace OSK::GRAPHICS {

	class ICommandPool;
	class ISyncDevice;

	/// <summary>
	/// Interfaz de una tarjeta gráfica.
	/// 
	/// @note Esta interfaz no implementa ningún código, se debe usar una instancia de
	/// GpuVulkan o GpuDx12.
	/// </summary>
	class OSKAPI_CALL IGpu {

	public:

		virtual ~IGpu() = default;

		/// <summary>
		/// Crea una pool de comandos, para poder crear listas de comandos.
		/// </summary>
		virtual OwnedPtr<ICommandPool> CreateCommandPool() = 0;

		/// <summary>
		/// Crea un dispositivo de sincronización GPU - CPU.
		/// </summary>
		virtual OwnedPtr<ISyncDevice> CreateSyncDevice() = 0;

		/// <summary>
		/// Elimina los recursos asociados a la GPU.
		/// </summary>
		virtual void Close() = 0;

		/// <summary>
		/// Castea la clase al tipo dado.
		/// 
		/// @note Este tipo debe ser una implementación de esta interfaz.
		/// </summary>
		template <typename T> T* As() const requires std::is_base_of_v<IGpu, T>{
			return (T*)this;
		}

	protected:

		IGpu() { }

	private:

	};

}
