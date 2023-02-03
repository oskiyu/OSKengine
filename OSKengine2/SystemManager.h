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

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS {

	/// @brief El SystemManager se encarga de almacenar y manejar los sistemas del juego.
	/// Es el encargado de llamar a las funciones OnTick de los sistemas, además
	/// de introducir y eliminar de los sistemas a los GameObjects que cada sistema
	/// vaya a procesar.
	/// 
	/// @note Dueño de los sistemas.
	class OSKAPI_CALL SystemManager {

	public:

		/// @brief Ejecuta la lógica OnTick de todos los sistemas.
		/// @param deltaTime Tiempo, en segundos, que ha pasado desde el último frame.
		void OnTick(TDeltaTime deltaTime);

		/// @brief Elimina el GameObject de todos los sistemas.
		/// @param obj Objeto eliminado.
		void GameObjectDestroyed(GameObjectIndex obj);

		
		/// @brief Añade o elimina el objeto de los sistemas, según sea conveniente
		/// dependiendo del nuevo signature.
		/// @param obj Objeto cuyo signature ha cambiado.
		/// @param signature Nuevo signature del objeto.
		void GameObjectSignatureChanged(GameObjectIndex obj, const Signature& signature);


		template <typename TSystem> requires IsProducerSystem<TSystem>
		TSystem* RegisterProducerSystem() {
			TSystem* sistema = new TSystem;

			producers.Insert(TSystem::GetSystemName(), (IProducerSystem*)sistema);

			return sistema;
		}

		template <typename TSystem> requires IsConsumerSystem<TSystem>
		TSystem* RegisterConsumerSystem() {
			TSystem* sistema = new TSystem;

			consumers.Insert(TSystem::GetSystemName(), (IConsumerSystem*)sistema);

			return sistema;
		}

		template <typename TSystem> requires IsPureSystem<TSystem>
		TSystem* RegisterPureSystem() {
			TSystem* sistema = new TSystem;

			systems.Insert(TSystem::GetSystemName(), (IPureSystem*)sistema);

			return sistema;
		}

		template <typename TSystem> requires IsEcsSystem<TSystem>
		TSystem* RegisterSystem() {
			TSystem* sistema = new TSystem;
			const std::string key = TSystem::GetSystemName();

			if constexpr (IsProducerSystem<TSystem>) {
				producers.Insert(key, (IProducerSystem*)sistema);
			}

			if constexpr (IsConsumerSystem<TSystem>) {
				consumers.Insert(key, (IConsumerSystem*)sistema);
			}

			if constexpr (IsPureSystem<TSystem>) {
				systems.Insert(key, (IPureSystem*)sistema);
			}

			return sistema;
		}

		/// <summary>
		/// Elimina el sistema dado, para que no sea procesado a partir de ahora.
		/// </summary>
		/// 
		/// @note Si el sistema no está registrado, no ocurre nada.
		/// 
		/// @bug No llama a ISystem::OnRemove().
		template <typename TSystem> requires IsEcsSystem<TSystem> 
		void RemoveSystem() {
			if constexpr (IsProducerSystem<TSystem>) {
				producers.Remove(TSystem::GetSystemName());
			}

			if constexpr (IsConsumerSystem<TSystem>) {
				consumers.Remove(TSystem::GetSystemName());
			}

			if constexpr (IsPureSystem<TSystem>) {
				systems.Remove(TSystem::GetSystemName());
			}
		}

		/// <summary>
		/// Devuelve la instancia del sistema dado.
		/// </summary>
		template <typename TSystem> requires IsEcsSystem<TSystem>
		TSystem* GetSystem() const {
			if constexpr (IsProducerSystem<TSystem>) {
				return (TSystem*)producers.Get(TSystem::GetSystemName()).GetPointer();
			}

			if constexpr (IsConsumerSystem<TSystem>) {
				return (TSystem*)consumers.Get(TSystem::GetSystemName()).GetPointer();
			}

			if constexpr (IsPureSystem<TSystem>) {
				return (TSystem*)systems.Get(TSystem::GetSystemName()).GetPointer();
			}
		}

		/// <summary>
		/// Comprueba si un sistema dado está registrado.
		/// </summary>
		template <typename TSystem> bool ContainsSystem() const {
			const std::string key = TSystem::GetSystemName();
			if constexpr (IsProducerSystem<TSystem>) {
				return producers.ContainsKey(key);
			}

			if constexpr (IsConsumerSystem<TSystem>) {
				return consumers.ContainsKey(key);
			}

			if constexpr (IsPureSystem<TSystem>) {
				return systems.ContainsKey(key);
			}
		}

	private:

		HashMap<std::string, UniquePtr<IProducerSystem>> producers;
		HashMap<std::string, UniquePtr<IConsumerSystem>> consumers;
		HashMap<std::string, UniquePtr<IPureSystem>> systems;

	};

}
