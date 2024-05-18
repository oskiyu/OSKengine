#include "SystemExecutionGraph.h"

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::ECS;


void SystemExecutionGraph::AddSystem(ISystem* system, const SystemDependencies& dependencies) {
	m_allRegisteredSystems[static_cast<std::string>(system->GetName())] = system;

	// OSK_ASSERT(Validate(), SystemCyclicDependencyException(system->GetName()));

	Rebuild();

	OSK::Engine::GetLogger()->InfoLog(std::format("Añadido sistema: {}", system->GetName()));
}

void SystemExecutionGraph::RemoveSystem(const ISystem* system) {
	m_allRegisteredSystems.erase((const std::string)system->GetName()); // @todo ??

	for (auto& set : m_flatExecutionGraph) {
		for (UIndex64 i = 0; i < set.systems.GetSize(); i++) {
			if (system == set.systems[i]) {
				set.systems.RemoveAt(i);
				return;
			}
		}
	}
}

bool SystemExecutionGraph::ValidatePreviousDependencies(std::string_view startPoint, const ISystem* system) const {
	if (!system) {
		return true;
	}

	if (system->GetName() == startPoint) {
		return false;
	}

	for (const auto& followingSystem : system->GetDependencies().executeAfterThese) {
		const auto* newSystem = m_allRegisteredSystems.contains(followingSystem)
			? m_allRegisteredSystems.find(followingSystem)->second
			: nullptr;

		if (!ValidatePreviousDependencies(startPoint, newSystem)) {
			return false;
		}
	}

	return true;
}

bool SystemExecutionGraph::ValidateFollowingDependencies(std::string_view startPoint, const ISystem* system) const {
	if (!system) {
		return true;
	}

	if (system->GetName() == startPoint) {
		return false;
	}

	for (const auto& followingSystem : system->GetDependencies().executeBeforeThese) {
		const auto* newSystem = m_allRegisteredSystems.contains(followingSystem)
			? m_allRegisteredSystems.find(followingSystem)->second
			: nullptr;

		if (!ValidateFollowingDependencies(startPoint, newSystem)) {
			return false;
		}
	}

	return true;
}

bool SystemExecutionGraph::Validate() const {
	for (const auto& [name, system] : m_allRegisteredSystems) {

		for (const auto& followingSystem : system->GetDependencies().executeBeforeThese) {
			const auto* nSystem = m_allRegisteredSystems.contains(followingSystem)
				? m_allRegisteredSystems.find(followingSystem)->second
				: nullptr;

			if (nSystem && !ValidateFollowingDependencies(name, nSystem)) {
				return false;
			}
		}
		
		for (const auto& previousSystem : system->GetDependencies().executeAfterThese) {
			const auto* nSystem = m_allRegisteredSystems.contains(previousSystem)
				? m_allRegisteredSystems.find(previousSystem)->second
				: nullptr;

			if (nSystem && !ValidatePreviousDependencies(name, nSystem)) {
				return false;
			}
		}
	}
}

void SystemExecutionGraph::Rebuild() {
	m_flatExecutionGraph.Empty();

	struct Entry {
		ISystem* system;
		bool valid = true;
	};
	std::list<Entry> orderedSystems;

	for (auto& [name, system] : m_allRegisteredSystems) {
		orderedSystems.push_back({ system });
	}

	bool dirty = true;
	UIndex64 count = 0;
	while (dirty) {
		dirty = false;

		for (auto it = orderedSystems.begin(); it != orderedSystems.end(); ++it) {
			if (!it->valid) {
				continue;
			}

			// Por detras:
			for (auto it2 = orderedSystems.begin(); it2 != it; ++it2) {
				if (!it2->valid) {
					continue;
				}

				auto* segundoSistema = it->system;
				auto* primerSistema = it2->system;

				if (primerSistema->GetDependencies().executeAfterThese.contains(segundoSistema->GetName()))
				{
					orderedSystems.insert(it2, { primerSistema });
					it->valid = false;
					dirty = true;

					break;
				}
			}

			if (dirty) {
				break;
			}

			// Por delante:
			for (auto it2 = it; it2 != orderedSystems.end(); ++it2) {
				if (!it2->valid) {
					continue;
				}

				auto* primerSistema = it->system;
				auto* segundoSistema = it2->system;

				if (primerSistema->GetDependencies().executeAfterThese.contains(segundoSistema->GetName()))
				{
					it2++;
					orderedSystems.insert(it2, { primerSistema });
					it->valid = false;
					dirty = true;

					break;
				}
			}

			count++;
		}
	}

	SystemExecutionSet executionSet{};
	for (const auto& [system, valid] : orderedSystems) {
		if (!valid) {
			continue;
		}

		bool needsNewSet = false;

		for (const auto* nSystem : executionSet.systems) {
			const bool barrier =
				system->GetDependencies().executeAfterThese.contains(nSystem->GetName());

			if (barrier || system->GetDependencies().exclusiveExecution || nSystem->GetDependencies().exclusiveExecution) {
				needsNewSet = true;
				break;
			}
		}

		if (needsNewSet) {
			m_flatExecutionGraph.Insert(executionSet);
			executionSet = {};
			executionSet.systems.Insert(system);
		}
		else {
			executionSet.systems.Insert(system);
		}
	}

	if (!executionSet.systems.IsEmpty()) {
		m_flatExecutionGraph.Insert(executionSet);
	}

	Engine::GetLogger()->InfoLog("Nuevo grafo:");
	for (const auto& set : m_flatExecutionGraph) {
		Engine::GetLogger()->InfoLog("\tNuevo set:");

		for (const auto& system : set.systems) {
			Engine::GetLogger()->InfoLog(std::format("\t\t{}", system->GetName()));
		}
	}
}

std::span<SystemExecutionSet> SystemExecutionGraph::GetExecutionGraph() {
	return m_flatExecutionGraph.GetFullSpan();
}

std::span<const SystemExecutionSet> SystemExecutionGraph::GetExecutionGraph() const {
	return m_flatExecutionGraph.GetFullSpan();
}
