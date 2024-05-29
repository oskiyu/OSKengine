#include "JobSystem.h"

#include "OSKengine.h"

#include <thread>
#include <glm/glm.hpp>

using namespace OSK;


JobSystem::JobSystem() : m_scheduledJobs(64) {
	m_numThreads = glm::max<USize32>(1, std::thread::hardware_concurrency() - 1);
	
	for (UIndex32 i = 0; i < m_numThreads; i++) {
		m_threads.Insert(std::jthread([&]() {
			while (m_isRunning) {
				auto job = m_scheduledJobs.TryDequeue();

				if (job.has_value()) {
					m_numCurrentlyWorkingThreads.fetch_add(1);
					job.value()->Execute();

					m_numCurrentlyWorkingThreads.fetch_sub(1);
					m_numCurrentlyWorkingThreads_perTag.Decrement(job.value()->GetName());

					for (const auto& tag : job.value()->GetTags()) {
						m_numCurrentlyWorkingThreads_perTag.Decrement(tag);
					}
				}
				else {
					std::unique_lock lock(m_wakingMutex);
					m_wakingConditional.wait(lock, [&]() { return !m_scheduledJobs.IsEmpty() || !m_isRunning; });
				}
			}
			}));
	}

	OSK::Engine::GetLogger()->InfoLog(std::format("JobSystem: número de hilos: {}", m_threads.GetSize()));
}

JobSystem::~JobSystem() {
	m_isRunning = false;
	m_wakingConditional.notify_all();
}


void JobSystem::ScheduleJob(UniquePtr<IJob> job) {
	const auto* ptr = job.GetPointer();

	m_scheduledJobs.Enqueue(std::move(job));

	m_numCurrentlyWorkingThreads_perTag.Increment(ptr->GetName());
	for (const auto& tag : ptr->GetTags()) {
		m_numCurrentlyWorkingThreads_perTag.Increment(tag);
	}

	m_wakingConditional.notify_one();
}

bool JobSystem::IsBusy() const {
	return !(m_numCurrentlyWorkingThreads.load() == 0 && m_scheduledJobs.IsEmpty());
}

void JobSystem::WaitForJobs(std::string_view jobTypeName) {
	WaitForTag(jobTypeName);
}

void JobSystem::WaitForTag(std::string_view tag) {
	while (!(m_numCurrentlyWorkingThreads_perTag.GetCount(tag) == 0 && m_scheduledJobs.IsEmpty())) {
		std::this_thread::yield();
	}
}

void JobSystem::WaitForAll() const {
	while (IsBusy()) {
		std::this_thread::yield();
	}
}

USize32 JobSystem::GetNumThreads() const {
	return m_numThreads + 1;
}
