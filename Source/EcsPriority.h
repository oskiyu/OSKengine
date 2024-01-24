#pragma once

#include "Assert.h"
#include "ISystem.h"
#include "InvalidObjectStateException.h"

#include <list>

namespace OSK::ECS {

	struct SistemPriorityQueue {

	public:

		void AddSystem(ISystem* system, DynamicArray<std::string> beforeSystem, DynamicArray<std::string> afterSystem) {
			auto it = systems.cbegin();

			for (; it != systems.cend(); ++it) {
				beforeSystem.Remove(static_cast<std::string>((*it)->GetName()));

				if (beforeSystem.IsEmpty()) {
					break;
				}
			}

			systems.emplace(++it, system);

			for (; it != systems.cend(); ++it) {
				afterSystem.Remove(static_cast<std::string>((*it)->GetName()));
			}

			OSK_ASSERT(afterSystem.IsEmpty(), InvalidObjectStateException("La lista de prioridades de sistemas tiene una dependencia cíclica."));
		}

		void AddSistemAtStart(ISystem* system) {
			systems.push_front(system);
		}

		void AddSistemAtEnd(ISystem* system) {
			systems.push_back(system);
		}

	private:

		std::list<ISystem*> systems;

	};

}
