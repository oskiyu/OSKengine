#pragma once

#include "ApiCall.h"
#include "JobQueue.h"
#include "HashMap.hpp"

#include "JobCountMap.h"

#include <thread>
#include <atomic>


namespace OSK {

	/// @brief Sistema de ejecución multihilo.
	/// Permite insertar trabajos que serán ejecutados de manera paralela.
	class OSKAPI_CALL JobSystem {

	public:

		/// @brief Crea un JobSystem vacío.
		/// Crea todos los hilos.
		JobSystem();

		~JobSystem();


		/// @brief Añade un trabajo que será ejecutado por el sistema.
		/// @param job Trabajo a ejecutar.
		/// @threadsafe
		void ScheduleJob(
			UniquePtr<IJob>&& job);

		/// @return True si hay algún trabajo siendo ejecutado,
		/// false si todos los hilos están libres.
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

		/// @return Número de hilos totales.
		/// = número de hilos del sistema de 
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
