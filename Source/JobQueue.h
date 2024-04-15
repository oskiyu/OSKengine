#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "IJob.h"
#include "DynamicArray.hpp"
#include <optional>

#include <mutex>


namespace OSK {

	/// @brief Cola que almacena trabajos a ser procesados por
	/// el sistema de trabajos.
	/// 
	/// Implementado mediante un ring-buffer que puede cambiar
	/// de tamaño en tiempo de ejecución.
	class OSKAPI_CALL JobQueue {

	public:

		/// @brief Crea la cola con el número máximo de elementos incial.
		/// EL número máximo puede ampliarse en tiempo de ejecución.
		/// @param maxElements Número máximo de elementos.
		explicit JobQueue(USize32 maxElements);


		/// @brief Añade un trabajo a la cola.
		/// @param job Trabajo a procesar en el futuro.
		/// @threadsafe
		void Enqueue(UniquePtr<IJob> job);

		/// @brief Intenta obtener un trabajo que no haya sido 
		/// obtenido antes.
		/// @return Vacío si la cola está vacía, trabajo en caso contrario.
		/// @threadsafe
		std::optional<UniquePtr<IJob>> TryDequeue();


		/// @return True si la cola está vacía.
		/// @threadsafe
		bool IsEmpty() const;

	private:

		void Resize();

		DynamicArray<UniquePtr<IJob>> m_jobs;
		UIndex64 m_head = 0;
		UIndex64 m_tail = 0;

		mutable std::mutex m_lock;

	};

}
