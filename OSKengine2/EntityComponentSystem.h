#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "ComponentManager.h"
#include "GameObjectManager.h"
#include "SystemManager.h"
#include "ISystem.h"
#include "IRenderSystem.h"
#include "Assert.h"

#include "OSKengine.h"
#include "Window.h"

namespace OSK::ECS {

	/// @brief Entity-Component-System es un paradigma de programación en el cual los objetos se dividen
	/// en 3 partes diferenciables:
	/// 
	///	- GameObject:
	///		- Representa un objeto en concreto.
	///		- Es simplemente un identificador único.
	/// - Componente:
	///		- Estructura que almacena datos específicos de un tipo dado de un GameObject.
	///		- Los componentes no ejecutan lógica alguna, y sólo son contenedores de información 
	///		relacionada con un GameObject.
	/// - Sistemas:
	///		- Sencarga de ejecutar lógica con los datos de los componentes.
	/// 
	/// Esta clase es la encargada de manejar este sistema, proporcionando una capa
	/// de abstracción sobre la implementación del sistema.
	/// 
	/// @note Toda operación que el juego quiera hacer sobre el ECS debe hacerse a través de
	/// esta clase.
	/// @note Esta clase es dueña de los sistemas.
	class EntityComponentSystem {

	public:

		EntityComponentSystem();

		/// @brief Ejecuta la lógica OnTick de todos los sistemas registrados.
		/// @param deltaTime Tiempo, en segundos, que ha pasado desde la
		/// última ejecución de esta función.
		void OnTick(TDeltaTime deltaTime);


		/// @brief Registra un tipo de componente.
		/// @tparam TComponent Tipo del componente.
		/// 
		/// @warning Todo componente que se quiera usar debe ser primero registrado.
		/// @pre TComponent debe validar IsEcsComponent.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		void RegisterComponent() {
			componentManager->RegisterComponent<TComponent>();
		}


		/// @brief Añade el componente dado al objeto.
		/// @tparam TComponent Tipo del componente.
		/// @param obj ID del objeto al que se va a añadir.
		/// @param component Componente a añadir.
		/// @return Referencia no estable al componente recién añadido.
		/// 
		/// @throws std::runtime_exception si el objeto ya tiene un componente del tipo.
		/// @pre TComponent debe validar IsEcsComponent.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		TComponent& AddComponent(GameObjectIndex obj, const TComponent& component) {
			OSK_ASSERT(!ObjectHasComponent<TComponent>(obj), "El objeto " + std::to_string(obj) + " ya tiene el componente " + TComponent::GetComponentTypeName() + ".");

			TComponent& oComponent = componentManager->AddComponent(obj, component);

			// Cambio de signature del objeto.
			Signature signature = gameObjectManager->GetSignature(obj);
			signature.SetTrue(componentManager->GetComponentType<TComponent>());
			gameObjectManager->SetSignature(obj, signature);

			systemManager->GameObjectSignatureChanged(obj, signature);

			return oComponent;
		}

		/// @brief Añade el componente dado al objeto.
		/// @tparam TComponent Tipo del componente.
		/// @param obj ID del objeto al que se va a añadir.
		/// @param component Componente a añadir.
		/// @return Referencia no estable al componente recién añadido.
		/// 
		/// @throws std::runtime_exception si el objeto ya tiene un componente del tipo.
		/// @pre TComponent debe validar IsEcsComponent.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		TComponent& AddComponent(GameObjectIndex obj, TComponent&& component) {
			OSK_ASSERT(!ObjectHasComponent<TComponent>(obj), "El objeto " + std::to_string(obj) + " ya tiene el componente " + TComponent::GetComponentTypeName() + ".");

			auto& oComponent = componentManager->AddComponentMove<TComponent>(obj, std::move(component));

			// Cambio de signature del objeto.
			Signature signature = gameObjectManager->GetSignature(obj);
			signature.SetTrue(componentManager->GetComponentType<TComponent>());
			gameObjectManager->SetSignature(obj, signature);

			systemManager->GameObjectSignatureChanged(obj, signature);

			return oComponent;
		}

		/// @brief Comprueba si el objeto dado tiene añadido un componente del tipo dado.
		/// @tparam TComponent Tipo del componente.
		/// @param obj Entidad comprobada.
		/// 
		/// @pre TComponent debe validar IsEcsComponent.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		bool ObjectHasComponent(GameObjectIndex obj) const {
			return gameObjectManager->GetSignature(obj).Get(
				componentManager->GetComponentType<TComponent>()
			);
		}

		
		/// @brief Elimina el componente del objeto.
		/// @tparam TComponent Tipo del componente.
		/// @param obj Objeto al que se va a quitar el componente.
		/// 
		/// @throws std::runtime_exception Si el objeto no tiene el componente del tipo dado.
		/// @pre TComponent debe validar IsEcsComponent.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		void RemoveComponent(GameObjectIndex obj) {
			OSK_ASSERT(ObjectHasComponent<TComponent>(obj), "El objeto " + std::to_string(obj) + " no tiene el componente " + TComponent::GetComponentTypeName() + ".");
			
			componentManager->RemoveComponent<TComponent>(obj);

			Signature signature = gameObjectManager->GetSignature(obj);
			signature.SetFalse(componentManager->GetComponentType<TComponent>());
			gameObjectManager->SetSignature(obj, signature);

			systemManager->GameObjectSignatureChanged(obj, signature);
		}

		/// @brief Devuelve una referencia no estable al componente del tipo dado del objeto.
		/// @tparam TComponent Tipo del componente.
		/// @param obj Objeto poseedor del componente.
		/// @return Referencia no estable al componente del tipo dado del objeto.
		/// 
		/// @throws std::runtime_exception Si el objeto no tiene el componente.
		/// @pre TComponent debe validar IsEcsComponent.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		TComponent& GetComponent(GameObjectIndex obj) const {
			OSK_ASSERT(ObjectHasComponent<TComponent>(obj), "El objeto " + std::to_string(obj) + " no tiene el componente " + TComponent::GetComponentTypeName() + ".");
			return componentManager->GetComponent<TComponent>(obj);
		}

		/// @tparam TComponent Tipo del componente.
		/// @return Código identificativo del componente dado.
		/// 
		/// @warning El componente tiene que haber sido registrado.
		/// @pre TComponent debe validar IsEcsComponent.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		ComponentType GetComponentType() const {
			return componentManager->GetComponentType<TComponent>();
		}

		/// <summary>
		/// Registra y almacena un nuevo sistema del tipo dado.
		/// </summary>
		template <typename TSystem> TSystem* RegisterSystem() {
			TSystem* output = systemManager->RegisterSystem<TSystem>();

			((ISystem*)output)->OnCreate();

			if constexpr (std::is_base_of_v<IRenderSystem, TSystem>) {
				renderSystems.Insert(reinterpret_cast<IRenderSystem*>(output));
			}

			return output;
		}

		/// <summary>
		/// Elimina un sistema.
		/// </summary>
		/// 
		/// @note Si el sistema no está registrado, no ocurre nada.
		template <typename TSystem> void RemoveSystem() {
			if constexpr (std::is_base_of_v<IRenderSystem, TSystem>) {
				renderSystems.Remove(reinterpret_cast<IRenderSystem*>(systemManager->GetSystem<TSystem>()));
			}

			systemManager->RemoveSystem<TSystem>();
		}

		/// <summary>
		/// Devuelve la instancia almacenada del sistema del tipo dado.
		/// </summary>
		template <typename TSystem> TSystem* GetSystem() const {
			return systemManager->GetSystem<TSystem>();
		}

		/// <summary>
		/// Spawnea un nuevo objeto, y devuelve su ID.
		///
		/// @warning ECS NO ES DUEÑO DEL OBJETO, el juego es el encargado
		/// de eliminarlo mediante DestroyObject().
		/// </summary>
		GameObjectIndex OSKAPI_CALL SpawnObject();

		/// <summary>
		/// Elimina un objeto.
		///
		/// @note Su ID cambia a 0 para indicar que ya no es una ID válida.
		/// </summary>
		void OSKAPI_CALL DestroyObject(GameObjectIndex* obj);

		/// <summary>
		/// Devuelve los render systems registrados.
		/// </summary>
		const OSKAPI_CALL DynamicArray<IRenderSystem*>& GetRenderSystems() const;

	private:

		UniquePtr<SystemManager> systemManager;
		UniquePtr<ComponentManager> componentManager;
		UniquePtr<GameObjectManager> gameObjectManager;

		/// <summary>
		/// @todo Pointer stability safety
		/// </summary>
		DynamicArray<IRenderSystem*> renderSystems;

	};

}
