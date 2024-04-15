#pragma once

#include "OSKmacros.h"
#include "HashMap.hpp"
#include "UniquePtr.hpp"
#include "GameObject.h"
#include "Component.h"

#include "IIteratorSystem.h"
#include "IConsumerSystem.h"

#include "SystemExecutionGraph.h"

#include <string>
#include <map>


namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS {

	class EventManager;


	/// @brief El SystemManager se encarga de almacenar y manejar los sistemas del juego.
	/// Es el encargado de llamar a las funciones OnTick de los sistemas, además
	/// de introducir y eliminar de los sistemas a los GameObjects que cada sistema
	/// vaya a procesar.
	/// 
	/// @note Dueño de los sistemas.
	class OSKAPI_CALL SystemManager {

	public:

		SystemManager() = default;
		OSK_DISABLE_COPY(SystemManager);
		OSK_DEFAULT_MOVE_OPERATOR(SystemManager);

		/// @brief Ejecuta la lógica OnTick de todos los sistemas.
		/// @param deltaTime Tiempo, en segundos, que ha pasado desde el último frame.
		void OnTick(TDeltaTime deltaTime, const EventManager& eventManager);

		/// @brief Ejecuta el renderizado de los sistemas de renderizado.
		/// @param commandList Lista de comandos.
		void OnRender(GRAPHICS::ICommandList* commandList);


		/// @brief Elimina el GameObject de todos los sistemas.
		/// @param obj Objeto eliminado.
		void GameObjectDestroyed(GameObjectIndex obj);

		
		/// @brief Añade o elimina el objeto de los sistemas, según sea conveniente
		/// dependiendo del nuevo signature.
		/// @param obj Objeto cuyo signature ha cambiado.
		/// @param signature Nuevo signature del objeto.
		void GameObjectSignatureChanged(GameObjectIndex obj, const Signature& signature);



		/// @brief Registra un sistema, con lo que se ejecutará
		/// a partir de ahora. Si ya estaba registrado, no ocurre nada.
		/// @tparam TSystem Tipo del sistema.
		/// @return Instancia del sistema.
		template <typename TSystem> requires IsEcsSystem<TSystem>
		TSystem* RegisterSystem(const SystemDependencies& dependencies) {
			const auto key = (std::string)TSystem::GetSystemName();

			if (this->ContainsSystem<TSystem>())
				return this->GetSystem<TSystem>();

			auto sistema = new TSystem;

			sistema->_SetDependencies(dependencies);

			SystemEntry entry{};
			entry.system = sistema;
			entry.compatibleObjects = {};

			m_systems[key] = std::move(entry);

			m_executionGraph.AddSystem(sistema, dependencies);

			return sistema;
		}

		/// @brief Elimina el sistema dado, para que no sea procesado a partir de ahora.
		/// @tparam TSystem Tipo del sistema.
		/// @note Si el sistema no está registrado, no ocurre nada.
		template <typename TSystem> requires IsEcsSystem<TSystem> 
		void RemoveSystem() {
			const auto* system = GetSystem<TSystem>();
			m_executionGraph.RemoveSystem(system);

			m_systems.erase((std::string)TSystem::GetSystemName());
		}

		/// @tparam TSystem Tipo del sistema.
		/// @return La instancia del sistema dado.
		/// Null si no está registrado.
		template <typename TSystem> requires IsEcsSystem<TSystem>
		TSystem* GetSystem() {
			if (!ContainsSystem<TSystem>()) {
				return nullptr;
			}

			return static_cast<TSystem*>(m_systems.find(TSystem::GetSystemName())->second.system.GetPointer());
		}

		/// @tparam TSystem Tipo del sistema.
		/// @return La instancia del sistema dado.
		/// Null si no está registrado.
		template <typename TSystem> requires IsEcsSystem<TSystem>
		const TSystem* GetSystem() const {
			if (!ContainsSystem<TSystem>()) {
				return nullptr;
			}

			return static_cast<const TSystem*>(m_systems.find(TSystem::GetSystemName())->second.system.GetPointer());
		}
				
		/// @brief Comprueba si un sistema dado está registrado, y por
		/// lo tanto, ejecutándose.
		/// @tparam TSystem Sistema.
		/// @return True si está registrado, false en caso contrario.
		template <typename TSystem> requires IsEcsSystem<TSystem> 
		bool ContainsSystem() const {
			return m_systems.contains(TSystem::GetSystemName());
		}


	private:

		void LaunchJobs(
			ISystem* system,
			TDeltaTime deltaTime, 
			const EventManager& eventManager);

		struct SystemEntry {
			UniquePtr<ISystem> system;
			DynamicArray<GameObjectIndex> compatibleObjects;
		};

		static void AddObject(SystemEntry* entry, GameObjectIndex obj);
		static void RemoveObject(SystemEntry* entry, GameObjectIndex obj);

		std::unordered_map<std::string, SystemEntry, StringHasher, std::equal_to<>> m_systems;
		SystemExecutionGraph m_executionGraph{};

	};

}
