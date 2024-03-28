#include "SystemManager.h"

#include "OSKengine.h"
#include "Logger.h"
#include "ISystem.h"
#include "EventManager.h"
#include "IRenderSystem.h"

using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

// #define OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS


void SystemManager::OnTick(TDeltaTime deltaTime, const EventManager& eventManager) {
#ifdef OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS
	Engine::GetLogger()->InfoLog("Nuevo frame.");
#endif // OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS

	for (const auto& executionSet : m_executionGraph.GetExecutionGraph()) {
#ifdef OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS
		Engine::GetLogger()->InfoLog("\tNuevo set.");
#endif // OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS

		for (auto* system : executionSet.systems) {
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

				consumerSystem->OnExecutionStart();
				consumerSystem->Execute(deltaTime, eventManager.GetEventQueueSpan(eventName));
				consumerSystem->OnExecutionEnd();
			}

#ifdef OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS
			Engine::GetLogger()->InfoLog(std::format("\t\tSistema ejecutado: {}", system->GetName()));
#endif // OSK_DEBUG_PRINT_SYSTEM_EXECUTIONS
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
