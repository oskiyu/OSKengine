#pragma once

#include "ApiCall.h"
#include "JobQueue.h"
#include "HashMap.hpp"

#include "JobCountMap.h"

#include <thread>
#include <atomic>


namespace OSK {

	/// @brief Sistema de ejecuci�n multihilo.
	/// Permite insertar trabajos que ser�n ejecutados de manera paralela.
	class OSKAPI_CALL JobSystem {

	public:

		/// @brief Crea un JobSystem vac�o.
		/// Crea todos los hilos.
		JobSystem();

		~JobSystem();


		/// @brief A�ade un trabajo que ser� ejecutado por el sistema.
		/// @param job Trabajo a ejecutar.
		/// @threadsafe
		void ScheduleJob(
			UniquePtr<IJob>&& job);

		/// @return True si hay alg�n trabajo siendo ejecutado,
		/// false si todos los hilos est�n libres.
		/// @threadsafe
		bool IsBusy() const;

		/// @brief Bloquea el hilo hasta que todos los trabajos
		/// encolados hayan terminado.
		/// @threadsafe
		void WaitForAll() const;


		/// @brief Bloquea el hilo hasta que todos los trabajos
		/// del tipo dado encolados hayan terminado.
		/// @threadsafe
		void WaitForJobs(std::string_view jobTypeName);

		/// @brief Bloquea el hilo hasta que todos los trabajos
		/// del tipo dado encolados hayan terminado.
		/// @tparam TJob Tipo de trabajo.
		/// @threadsafe
		template <typename TJob>
			requires IsJob<TJob>
		void WaitForJobs() {
			WaitForJobs(TJob::GetTypeName());
		}

		/// @brief Bloquea el hilo hasta que todos los trabajos
		/// con el tag dado encolados hayan terminado.
		/// @param tag Tag del trabajo.
		/// @threadsafe
		void WaitForTag(std::string_view tag);

		/// @return N�mero de hilos totales.
		/// = n�mero de hilos del sistema de 
		/// trabajos + 1 (hilo principal).
		USize32 GetNumThreads() const;

	private:

		bool m_isRunning = true;

		USize32 m_numThreads = 0;
		JobQueue m_scheduledJobs;

		DynamicArray<std::jthread> m_threads;

		std::mutex m_wakingMutex;
		std::condition_variable m_wakingConditional;

		std::atomic<USize32> m_numCurrentlyWorkingThreads;

		JobCountMap m_numCurrentlyWorkingThreads_perTag;

	};

}
