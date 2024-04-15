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
	/// de tama�o en tiempo de ejecuci�n.
	class OSKAPI_CALL JobQueue {

	public:

		/// @brief Crea la cola con el n�mero m�ximo de elementos incial.
		/// EL n�mero m�ximo puede ampliarse en tiempo de ejecuci�n.
		/// @param maxElements N�mero m�ximo de elementos.
		explicit JobQueue(USize32 maxElements);


		/// @brief A�ade un trabajo a la cola.
		/// @param job Trabajo a procesar en el futuro.
		/// @threadsafe
		void Enqueue(UniquePtr<IJob> job);

		/// @brief Intenta obtener un trabajo que no haya sido 
		/// obtenido antes.
		/// @return Vac�o si la cola est� vac�a, trabajo en caso contrario.
		/// @threadsafe
		std::optional<UniquePtr<IJob>> TryDequeue();


		/// @return True si la cola est� vac�a.
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
