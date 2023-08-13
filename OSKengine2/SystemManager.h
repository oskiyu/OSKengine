#pragma once

#include "OSKmacros.h"
#include "HashMap.hpp"
#include "UniquePtr.hpp"
#include "GameObject.h"
#include "Component.h"

#include "IPureSystem.h"
#include "IConsumerSystem.h"
#include "IProducerSystem.h"

#include <string>
#include <map>

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS {

	/// @brief El SystemManager se encarga de almacenar y manejar los sistemas del juego.
	/// Es el encargado de llamar a las funciones OnTick de los sistemas, adem�s
	/// de introducir y eliminar de los sistemas a los GameObjects que cada sistema
	/// vaya a procesar.
	/// 
	/// @note Due�o de los sistemas.
	class OSKAPI_CALL SystemManager {

	private:

		/// @brief Conjunto con los sistemas que tienen un mismo �rden
		/// de ejecuci�n.
		struct SystemSet {
			DynamicArray<IProducerSystem*> producers;
			DynamicArray<IConsumerSystem*> consumers;
			DynamicArray<IPureSystem*> systems;
		};

		/// @brief Indica el conjunto de sistemas que tienen
		/// un mismo orden de ejecuci�n.
		std::map<int, SystemSet> executionOrder;

	public:

		SystemManager() = default;
		OSK_DISABLE_COPY(SystemManager);
		OSK_DEFAULT_MOVE_OPERATOR(SystemManager);

		/// @brief Ejecuta la l�gica OnTick de todos los sistemas.
		/// @param deltaTime Tiempo, en segundos, que ha pasado desde el �ltimo frame.
		void OnTick(TDeltaTime deltaTime);

		/// @brief Elimina el GameObject de todos los sistemas.
		/// @param obj Objeto eliminado.
		void GameObjectDestroyed(GameObjectIndex obj);

		
		/// @brief A�ade o elimina el objeto de los sistemas, seg�n sea conveniente
		/// dependiendo del nuevo signature.
		/// @param obj Objeto cuyo signature ha cambiado.
		/// @param signature Nuevo signature del objeto.
		void GameObjectSignatureChanged(GameObjectIndex obj, const Signature& signature);



		/// @brief Registra un sistema, con lo que se ejecutar�
		/// a partir de ahora. Si ya estaba registrado, no ocurre nada.
		/// @tparam TSystem Tipo del sistema.
		/// @return Instancia del sistema.
		template <typename TSystem> requires IsEcsSystem<TSystem>
		TSystem* RegisterSystem(int order) {
			const auto key = (std::string)TSystem::GetSystemName();

			if (this->ContainsSystem<TSystem>())
				return this->GetSystem<TSystem>();

			auto sistema = new TSystem;

			if constexpr (IsProducerSystem<TSystem>) {
				producers[key] = (IProducerSystem*)sistema;
			}

			if constexpr (IsConsumerSystem<TSystem>) {
				consumers[key] = (IConsumerSystem*)sistema;
			}

			if constexpr (IsPureSystem<TSystem>) {
				systems[key] = (IPureSystem*)sistema;
			}

			this->SetExecutionOrder(order, sistema);

			return sistema;
		}

		/// @brief Elimina el sistema dado, para que no sea procesado a partir de ahora.
		/// @tparam TSystem Tipo del sistema.
		/// @note Si el sistema no est� registrado, no ocurre nada.
		template <typename TSystem> requires IsEcsSystem<TSystem> 
		void RemoveSystem() {
			TSystem* system = GetSystem<TSystem>();

			SystemSet& systemSet = executionOrder.at(system->GetExecutionOrder());

			if constexpr (IsProducerSystem<TSystem>) {
				systemSet.producers.Remove(system);
				producers.erase(TSystem::GetSystemName());
			}

			if constexpr (IsConsumerSystem<TSystem>) {
				systemSet.consumers.Remove(system);
				consumers.erase(TSystem::GetSystemName());
			}

			if constexpr (IsPureSystem<TSystem>) {
				systemSet.systems.Remove(system);
				systems.erase((std::string)TSystem::GetSystemName());
			}
		}

		/// @tparam TSystem Tipo del sistema.
		/// @return La instancia del sistema dado.
		/// Null si no est� registrado.
		template <typename TSystem> requires IsEcsSystem<TSystem>
		TSystem* GetSystem() {
			if constexpr (IsProducerSystem<TSystem>) {
				return (TSystem*)producers.find(TSystem::GetSystemName())->second.GetPointer();
			}

			if constexpr (IsConsumerSystem<TSystem>) {
				return (TSystem*)consumers.find(TSystem::GetSystemName())->second.GetPointer();
			}

			if constexpr (IsPureSystem<TSystem>) {
				return (TSystem*)systems.find(TSystem::GetSystemName())->second.GetPointer();
			}

			return nullptr;
		}

		/// @tparam TSystem Tipo del sistema.
		/// @return La instancia del sistema dado.
		/// Null si no est� registrado.
		template <typename TSystem> requires IsEcsSystem<TSystem>
		const TSystem* GetSystem() const {
			if constexpr (IsProducerSystem<TSystem>) {
				return (const TSystem*)producers.find(TSystem::GetSystemName())->second.GetPointer();
			}

			if constexpr (IsConsumerSystem<TSystem>) {
				return (const TSystem*)consumers.find(TSystem::GetSystemName())->second.GetPointer();
			}

			if constexpr (IsPureSystem<TSystem>) {
				return (const TSystem*)systems.find(TSystem::GetSystemName())->second.GetPointer();
			}

			return nullptr;
		}
				
		/// @brief Comprueba si un sistema dado est� registrado, y por
		/// lo tanto, ejecut�ndose.
		/// @tparam TSystem Sistema.
		/// @return True si est� registrado, false en caso contrario.
		template <typename TSystem> requires IsEcsSystem<TSystem> 
		bool ContainsSystem() const {
			if constexpr (IsProducerSystem<TSystem>) {
				return producers.contains(TSystem::GetSystemName());
			}

			if constexpr (IsConsumerSystem<TSystem>) {
				return consumers.contains(TSystem::GetSystemName());
			}

			if constexpr (IsPureSystem<TSystem>) {
				return systems.contains(TSystem::GetSystemName());
			}

			return false;
		}


	private:

		/// @brief Actualiza el orden de ejecuci�n de un sistema,
		/// introduci�ndolo en el set de ejecuci�n indicado.
		/// @tparam TSystem Tipo del sistema.
		/// @param order Nuevo orden de ejecuci�n.
		/// @param sistema Sistema a modificar.
		/// @post @p sistema tendr� su orden de ejecuci�n = @p order .
		/// @post @p sistema estar� incluido en el set de ejecuci�n @p order .
		/// @post @p sistema no estar� incluido en el set de ejecuci�n en el que estaba antes.
		template <typename TSystem> requires IsEcsSystem<TSystem>
		void SetExecutionOrder(int order, TSystem* sistema) {

			// Se quita el sistema del anterior set de ejecuci�n (si existe).
			if (executionOrder.contains(sistema->GetExecutionOrder())) {
				SystemSet& oldExecutionSet = executionOrder.at(sistema->GetExecutionOrder());

				if constexpr (IsProducerSystem<TSystem>) {
					oldExecutionSet.producers.Remove((IProducerSystem*)sistema);
				}

				if constexpr (IsConsumerSystem<TSystem>) {
					oldExecutionSet.consumers.Remove((IConsumerSystem*)sistema);
				}

				if constexpr (IsPureSystem<TSystem>) {
					oldExecutionSet.systems.Remove((IPureSystem*)sistema);
				}
			}

			if (!executionOrder.contains(order))
				executionOrder[order] = SystemSet();

			SystemSet& executionSet = executionOrder.at(order);

			if constexpr (IsProducerSystem<TSystem>) {
				executionSet.producers.Insert((IProducerSystem*)sistema);
			}

			if constexpr (IsConsumerSystem<TSystem>) {
				executionSet.consumers.Insert((IConsumerSystem*)sistema);
			}

			if constexpr (IsPureSystem<TSystem>) {
				executionSet.systems.Insert((IPureSystem*)sistema);
			}

			sistema->_SetExecutionOrder(order);
		}

		std::unordered_map<std::string, UniquePtr<IProducerSystem>, StringHasher, std::equal_to<>> producers;
		std::unordered_map<std::string, UniquePtr<IConsumerSystem>, StringHasher, std::equal_to<>> consumers;
		std::unordered_map<std::string, UniquePtr<IPureSystem>, StringHasher, std::equal_to<>> systems;

	};

}
