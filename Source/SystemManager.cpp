#include "SystemManager.h"

#include "OSKengine.h"
#include "ISystem.h"
#include "EventManager.h"
#include "IRenderSystem.h"

#include "IteratorSystemExecutionJob.h"
#include "ConsumerSystemExecutionJob.h"

#include "StopWatch.h"

using namespace OSK::ECS;
using namespace OSK::GRAPHICS;


void SystemManager::OnTick(TDeltaTime deltaTime, const EventManager& eventManager) {
	static float min = std::numeric_limits<float>::max();
	static float max = std::numeric_limits<float>::lowest();
	static float averaged = 0.0f;
	static USize32 count = 0;

	StopWatch stopWatch{};
	stopWatch.Start();

// #define OSK_SINGLE_THREAD
#ifdef OSK_SINGLE_THREAD
	for (const auto& executionSet : m_executionGraph.GetExecutionGraph()) {

		// OnStarts
		for (auto* system : executionSet.systems) {
			if (!system->IsActive()) {
				continue;
			}

			if (system->Is<IIteratorSystem>()) {
				auto* iteratorSystem = system->As<IIteratorSystem>();
				const auto compatibleObjects = m_systems.find(system->GetName())->second.compatibleObjects.GetFullSpan();

				iteratorSystem->OnExecutionStart();
				iteratorSystem->Execute(deltaTime, compatibleObjects);
				iteratorSystem->OnExecutionEnd();
			}

			if (system->Is<IConsumerSystem>()) {
				auto* consumerSystem = system->As<IConsumerSystem>();
				const auto eventName = consumerSystem->GetEventName();
				const auto events = eventManager.GetEventQueueSpan(eventName);

				consumerSystem->OnExecutionStart();
				consumerSystem->Execute(deltaTime, events);
				consumerSystem->OnExecutionEnd();
			}
		}
	}
#else
	for (const auto& executionSet : m_executionGraph.GetExecutionGraph()) {

		// OnStarts
		for (auto* system : executionSet.systems) {
			if (!system->IsActive()) {
				continue;
			}

			if (system->Is<IIteratorSystem>()) {
				auto* iteratorSystem = system->As<IIteratorSystem>();
				iteratorSystem->OnExecutionStart();
			}

			if (system->Is<IConsumerSystem>()) {
				auto* consumerSystem = system->As<IConsumerSystem>();
				consumerSystem->OnExecutionStart();
			}
		}

		// Jobs
		for (auto* system : executionSet.systems) {
			if (!system->IsActive()) {
				continue;
			}

			if (system->GetDependencies().singleThreaded) {
				if (system->Is<IIteratorSystem>()) {
					system->As<IIteratorSystem>()->Execute(deltaTime, 
						m_systems.find(system->GetName())->second.compatibleObjects.GetFullSpan());
				}

				if (system->Is<IConsumerSystem>()) {
					const auto eventName = system->As<IConsumerSystem>()->GetEventName();

					const auto events = eventManager.GetEventQueueSpan(eventName);
					system->As<IConsumerSystem>()->Execute(deltaTime, events);
				}
			}
			else {
				LaunchJobs(system, deltaTime, eventManager);
			}
		}

		Engine::GetJobSystem()->WaitForJobs<IteratorSystemExecutionJob>();
		Engine::GetJobSystem()->WaitForJobs<ConsumerSystemExecutionJob>();

		// OnEnds
		for (auto* system : executionSet.systems) {
			if (!system->IsActive()) {
				continue;
			}

			if (system->Is<IIteratorSystem>()) {
				auto* iteratorSystem = system->As<IIteratorSystem>();
				iteratorSystem->OnExecutionEnd();
			}

			if (system->Is<IConsumerSystem>()) {
				auto* consumerSystem = system->As<IConsumerSystem>();
				consumerSystem->OnExecutionEnd();
			}
		}
	}
#endif // OSK_SINGLE_THREAD

	stopWatch.Stop();

#ifdef OSK_SMANAGER_SHOW_LOGS
	min = glm::min(min, stopWatch.GetElapsedTime());
	max = glm::max(max, stopWatch.GetElapsedTime());
	averaged = count > 10
		? glm::mix(averaged, stopWatch.GetElapsedTime(), 0.01f)
		: glm::mix(averaged, stopWatch.GetElapsedTime(), 0.9f);

	count++;

	if (count % 100 == 0) {
		Engine::GetLogger()->InfoLog(std::format("ECS OnTick():"));
		Engine::GetLogger()->InfoLog(std::format("\tmin: {} ms", min * 1000.f));
		Engine::GetLogger()->InfoLog(std::format("\tmax: {} ms", max * 1000.f));
		Engine::GetLogger()->InfoLog(std::format("\taveraged: {} ms", averaged * 1000.f));
		Engine::GetLogger()->InfoLog(std::format("\treal: {} ms", stopWatch.GetElapsedTime() * 1000.f));
	}
#endif // OSK_SMANAGER_SHOW_LOGS
}

void SystemManager::LaunchJobs(ISystem* system, TDeltaTime deltaTime, const EventManager& eventManager) {
	const USize32 workgroupSize = 5;

	if (system->Is<IIteratorSystem>()) {
		auto* iteratorSystem = system->As<IIteratorSystem>();
		const auto compatibleObjects = m_systems.find(system->GetName())->second.compatibleObjects.GetFullSpan();

		for (UIndex64 i = 0; i < compatibleObjects.size(); i += workgroupSize) {
			if (i > compatibleObjects.size()) {
				break;
			}

			const USize64 numElements = glm::min<USize64>(compatibleObjects.size() - i, workgroupSize);

			Engine::GetJobSystem()->ScheduleJob(
				MakeUnique<IteratorSystemExecutionJob>(
					iteratorSystem,
					compatibleObjects.subspan(i, numElements),
					deltaTime
				)
			);
		}

	}

	if (system->Is<IConsumerSystem>()) {
		auto* consumerSystem = system->As<IConsumerSystem>();
		const auto eventName = consumerSystem->GetEventName();
		const auto events = eventManager.GetEventQueueSpan(eventName);

		for (UIndex64 i = 0; i < events.numEntries; i += workgroupSize) {
			if (i > events.numEntries) {
				break;
			}

			const USize64 numElements = glm::min<USize64>(events.numEntries - i, workgroupSize);

			Engine::GetJobSystem()->ScheduleJob(
				MakeUnique<ConsumerSystemExecutionJob>(
					consumerSystem,
					events.GetSubspan(i, numElements),
					deltaTime
				)
			);
		}
	}
}

void SystemManager::OnRender(ICommandList* commandList) {
#ifdef OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS
	Engine::GetLogger()->InfoLog("Nuevo frame renderizado.");
#endif // OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS
	
	for (const auto& executionSet : m_executionGraph.GetExecutionGraph()) {
#ifdef OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS
		Engine::GetLogger()->InfoLog("\tNuevo set.");
#endif // OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS

		for (auto* system : executionSet.systems) {
			if (!system->IsActive()) {
				continue;
			}

			if (system->Is<IRenderSystem>()) {
				auto* renderSystem = system->As<IRenderSystem>();
				const auto compatibleObjects = m_systems.find(system->GetName())->second.compatibleObjects.GetFullSpan();

				renderSystem->OnRenderStart(commandList);
				renderSystem->Render(commandList, compatibleObjects);
				renderSystem->OnRenderEnd(commandList);

#ifdef OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS
				Engine::GetLogger()->InfoLog(std::format("\t\tSistema renderizado: {}", system->GetName()));
#endif // OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS				
			}
		}
	}
}

void SystemManager::GameObjectDestroyed(GameObjectIndex obj) {
	for (auto& [name, entry] : m_systems) {
		RemoveObject(&entry, obj);
	}
}

void SystemManager::GameObjectSignatureChanged(GameObjectIndex obj, const Signature& signature) {
	for (auto& [name, entry] : m_systems) {
		if (entry.system->Is<IIteratorSystem>()) {
			if (entry.system->As<IIteratorSystem>()->GetSignature().IsCompatible(signature)) {
				AddObject(&entry, obj);
			}
			else {
				RemoveObject(&entry, obj);
			}
		}
	}
}

void SystemManager::AddObject(SystemEntry* entry, GameObjectIndex obj) {
	if (!entry->compatibleObjects.ContainsElement(obj)) {
		entry->compatibleObjects.Insert(obj);
		entry->system->As<IIteratorSystem>()->OnObjectAdded(obj);
		entry->system->As<IIteratorSystem>()->SetCompatibleObjects(entry->compatibleObjects.GetFullSpan());
	}
}

void SystemManager::RemoveObject(SystemEntry* entry, GameObjectIndex obj) {
	if (entry->compatibleObjects.ContainsElement(obj)) {
		entry->compatibleObjects.Remove(obj);
		entry->system->As<IIteratorSystem>()->OnObjectRemoved(obj);
		entry->system->As<IIteratorSystem>()->SetCompatibleObjects(entry->compatibleObjects.GetFullSpan());
	}
}

const SystemExecutionGraph& SystemManager::GetExecutionGraph() const {
	return m_executionGraph;
}

ISystem* SystemManager::GetSystem(std::string_view systemName) {
	auto iterator = m_systems.find(systemName);

	OSK_ASSERT(
		iterator != m_systems.end(),
		InvalidArgumentException(std::format("El sistema {} no se encuentra.", systemName)));

	return iterator->second.system.GetPointer();
}

const ISystem* SystemManager::GetSystem(std::string_view systemName) const {
	auto iterator = m_systems.find(systemName);

	OSK_ASSERT(
		iterator != m_systems.end(),
		InvalidArgumentException(std::format("El sistema {} no se encuentra.", systemName)));

	return iterator->second.system.GetPointer();
}

void SystemManager::DeactivateAllSystems() {
	for (auto& [name, system] : m_systems) {
		system.system->Deactivate();
	}
}
