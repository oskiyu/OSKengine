#pragma once

#include "ApiCall.h"
#include "HashMap.hpp"

// Para `ICommandPool`.
#include "UniquePtr.hpp"
#include "ICommandPool.h"

// Sincronización.
#include <shared_mutex>


namespace OSK::GRAPHICS {

	class ICommandQueue;


	/// @brief Mapa que enlaza un hilo con un command pool
	/// de un tipo dado.
	/// 
	/// Todos los pools del mapa son del mismo tipo.
	/// 
	/// @threadsafe
	class OSKAPI_CALL ThreadedCommandPoolMap {

	public:

		/// @brief Crea el mapa de pools.
		/// @param linkedCommandQueue Tipo de cola sobre la que
		/// se deben insertar las listas creadas por los pools
		/// de este mapa.
		/// 
		/// @pre @p linkedCommandQueue debe ser un puntero estable.
		explicit ThreadedCommandPoolMap(const ICommandQueue* linkedCommandQueue);

		OSK_DISABLE_COPY(ThreadedCommandPoolMap);


		/// @param threadId ID del hilo que esta tratando de obtener el command pool.
		/// @return Command pool exclusivo del hilo.
		/// 
		/// @threadsafe
		/// @stablepointer
		ICommandPool* GetCommandPool(std::thread::id threadId);

	private:

		std::unordered_map<std::thread::id, UniquePtr<ICommandPool>> m_commandPoolPerThread;
		std::shared_mutex m_readWriteMutex;

		const ICommandQueue* m_linkedCommandQueue;

	};

}
