#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"

#include "ComponentManager.h"
#include "GameObjectManager.h"
#include "SystemManager.h"
#include "EventManager.h"
#include "ISystem.h"
#include "IRenderSystem.h"
#include "Assert.h"

#include "SystemAlreadyRegisteredException.h"
#include "EcsExceptions.h"

namespace OSK::IO {
	class ILogger;
}

namespace OSK::ECS {

	/// @brief Entity-Component-System es un paradigma de programaci�n en el cual los objetos se dividen
	/// en 3 partes diferenciables:
	/// 
	///	- GameObject:
	///		- Representa un objeto en concreto.
	///		- Es simplemente un identificador �nico.
	/// - Componente:
	///		- Estructura que almacena datos espec�ficos de un tipo dado de un GameObject.
	///		- Los componentes no ejecutan l�gica alguna, y s�lo son contenedores de informaci�n 
	///		relacionada con un GameObject.
	/// - Sistemas:
	///		- Sencarga de ejecutar l�gica con los datos de los componentes.
	/// 
	/// Esta clase es la encargada de manejar este sistema, proporcionando una capa
	/// de abstracci�n sobre la implementaci�n del sistema.
	/// 
	/// @note Toda operaci�n que el juego quiera hacer sobre el ECS debe hacerse a trav�s de
	/// esta clase.
	/// @note Esta clase es due�a de los sistemas.
	class EntityComponentSystem final {

	public:

		/// @brief Inicializa todos los managers.
		explicit EntityComponentSystem(IO::ILogger* logger);

		/// @brief Ejecuta la l�gica OnTick de todos los sistemas registrados.
		/// @param deltaTime Tiempo, en segundos, que ha pasado desde la
		/// �ltima ejecuci�n de esta funci�n.
		void OnTick(TDeltaTime deltaTime);

#pragma region Components

		/// @brief Registra un tipo de componente.
		/// @tparam TComponent Tipo del componente.
		/// 
		/// @pre TComponent debe validar IsEcsComponent.
		/// @pre El componente no debe haber sido previamente registrado.
		/// 
		/// @throws ComponentAlreadyRegisteredException si el componente ya fue previamente registrado.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		void RegisterComponent() {
			OSK_ASSERT(
				!componentManager->ComponentHasBeenRegistered<TComponent>(), 
				ComponentAlreadyRegisteredException(TComponent::GetComponentTypeName()))

			componentManager->RegisterComponent<TComponent>();
		}

		/// @brief A�ade el componente dado al objeto.
		/// @tparam TComponent Tipo del componente.
		/// @param obj ID del objeto al que se va a a�adir.
		/// @param component Componente a a�adir.
		/// @return Referencia no estable al componente reci�n a�adido.
		/// 
		/// @pre TComponent debe validar IsEcsComponent.
		/// @pre El objeto no debe tener previamente un componente del tipo dado.
		/// 
		/// @throws ObjectAlreadyHasComponentException si el objeto ya tiene un componente del tipo.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		TComponent& AddComponent(GameObjectIndex obj, const TComponent& component) {
			EnsureRegistered<TComponent>();

			const bool objectHasComponent = ObjectHasComponent<TComponent>(obj);
			OSK_ASSERT(!objectHasComponent, ObjectAlreadyHasComponentException(TComponent::GetComponentTypeName(), obj))

			TComponent& oComponent = componentManager->AddComponent(obj, component);

			// Cambio de signature del objeto.
			Signature signature = gameObjectManager->GetSignature(obj);
			signature.SetTrue(componentManager->GetComponentType<TComponent>());
			gameObjectManager->SetSignature(obj, signature);

			systemManager->GameObjectSignatureChanged(obj, signature);

			return oComponent;
		}

		/// @brief A�ade el componente dado al objeto.
		/// @tparam TComponent Tipo del componente.
		/// @param obj ID del objeto al que se va a a�adir.
		/// @param component Componente a a�adir.
		/// @return Referencia no estable al componente reci�n a�adido.
		/// 
		/// @pre El objeto no debe tener previamente un componente del tipo dado.
		/// 
		/// @throws ObjectAlreadyHasComponentException si el objeto ya tiene un componente del tipo.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		TComponent& AddComponent(GameObjectIndex obj, TComponent&& component) {
			EnsureRegistered<TComponent>();

			const bool objectHasComponent = ObjectHasComponent<TComponent>(obj);
			OSK_ASSERT(!objectHasComponent, ObjectAlreadyHasComponentException(TComponent::GetComponentTypeName(), obj))
			
			auto& oComponent = componentManager->AddComponentMove<TComponent>(obj, std::move(component));

			// Cambio de signature del objeto.
			Signature signature = gameObjectManager->GetSignature(obj);
			signature.SetTrue(componentManager->GetComponentType<TComponent>());
			gameObjectManager->SetSignature(obj, signature);

			systemManager->GameObjectSignatureChanged(obj, signature);

			return oComponent;
		}

		/// @brief Comprueba si el objeto dado tiene a�adido un componente del tipo dado.
		/// @tparam TComponent Tipo del componente.
		/// @param obj Entidad comprobada.
		/// 
		/// @pre TComponent debe validar IsEcsComponent.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		bool ObjectHasComponent(GameObjectIndex obj) const {
			OSK_ASSERT(componentManager->ComponentHasBeenRegistered<TComponent>(), ComponentNotRegisteredException(TComponent::GetComponentTypeName()));

			return gameObjectManager->GetSignature(obj).Get(
				componentManager->GetComponentType<TComponent>()
			);
		}
				
		/// @brief Elimina el componente del objeto.
		/// @tparam TComponent Tipo del componente.
		/// @param obj Objeto al que se va a quitar el componente.
		/// 
		/// @pre TComponent debe validar IsEcsComponent.
		/// @pre El objeto debe tener previamente un componente del tipo dado.
		/// 
		/// @throws ComponentNotFoundException Si el objeto no tiene el componente del tipo dado.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		void RemoveComponent(GameObjectIndex obj) {
			EnsureRegistered<TComponent>();

			OSK_ASSERT(ObjectHasComponent<TComponent>(obj), ComponentNotFoundException(TComponent::GetComponentTypeName(), obj))
			
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
		/// @pre TComponent debe validar IsEcsComponent.
		/// @pre El objeto debe tener previamente un componente del tipo dado.
		/// 
		/// @throws ComponentNotRegisteredException Si el tipo de componente no ha sido registrado.
		/// @throws ComponentNotFoundException Si el objeto no tiene el componente del tipo dado.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		TComponent& GetComponent(GameObjectIndex obj) const {
			OSK_ASSERT(componentManager->ComponentHasBeenRegistered<TComponent>(), ComponentNotRegisteredException(TComponent::GetComponentTypeName()));
			OSK_ASSERT(ObjectHasComponent<TComponent>(obj), ComponentNotFoundException(TComponent::GetComponentTypeName(), obj))

			return componentManager->GetComponent<TComponent>(obj);
		}

		/// @tparam TComponent Tipo del componente.
		/// @return C�digo identificativo del componente dado.
		/// 
		/// @pre TComponent debe validar IsEcsComponent.
		/// 
		/// @throws ComponentNotRegisteredException Si el tipo de componente no ha sido previamente registrado.
		template <typename TComponent> requires IsEcsComponent<TComponent>
		ComponentType GetComponentType() const {
			OSK_ASSERT(componentManager->ComponentHasBeenRegistered<TComponent>(), ComponentNotRegisteredException(TComponent::GetComponentTypeName()));
			return componentManager->GetComponentType<TComponent>();
		}

#pragma endregion

#pragma region System

		/// @brief Registra y almacena un nuevo sistema del tipo dado.
		/// @tparam TSystem Tipo de sistema.
		/// @return Sistema creado.
		/// 
		/// @pre @p TSystem debe cumplir con IsEcsSystem<TSystem>.
		/// @pre El sistema no debe haber sido previamente registrado.
		/// 
		/// @throws SystemAlreadyRegisteredException si el sistema ya habia sido previamente registrado.
		template <typename TSystem> requires IsEcsSystem<TSystem>
		TSystem* RegisterSystem() {
			OSK_ASSERT(!systemManager->ContainsSystem<TSystem>(), SystemAlreadyRegisteredException(TSystem::GetSystemName()))

			TSystem* output = systemManager->RegisterSystem<TSystem>();

			((ISystem*)output)->OnCreate();

			if constexpr (std::is_base_of_v<IRenderSystem, TSystem>) {
				renderSystems.Insert(reinterpret_cast<IRenderSystem*>(output));
			}

			return output;
		}

		/// @brief Elimina un sistema, si est� registrado.
		/// @tparam TSystem Tipo de sistema.
		/// 
		/// @throws SystemNotFoundException Si el sistema no est� registrado.
		template <typename TSystem> void RemoveSystem() {
			OSK_ASSERT(
				HasSystem<TSystem>(),
				SystemNotFoundException(TSystem::GetSystemName()))

			if constexpr (std::is_base_of_v<IRenderSystem, TSystem>) {
				renderSystems.Remove(reinterpret_cast<IRenderSystem*>(systemManager->GetSystem<TSystem>()));
			}

			systemManager->RemoveSystem<TSystem>();
		}

		/// @brief Devuelve la instancia almacenada del sistema del tipo dado.
		/// @tparam TSystem Tipo de sistema.
		/// @return Sistema.
		/// 
		/// @pre El sistema debe haber sido previamente registrado con RegisterSystem.
		/// @throws SystemNotFoundException Si el sistema no est� registrado.
		template <typename TSystem> requires IsEcsSystem<TSystem>
		TSystem* GetSystem() const {
			OSK_ASSERT(
				HasSystem<TSystem>(),
				SystemNotFoundException(TSystem::GetSystemName()))

			return systemManager->GetSystem<TSystem>();
		}

		/// @tparam TSystem Tipo del sistema.
		/// @return True si el sistema est� registrado y se est� ejecutando.
		/// False en caso contrario.
		template <typename TSystem> requires IsEcsSystem<TSystem>
		bool HasSystem() const {
			return systemManager->ContainsSystem<TSystem>();
		}

#pragma endregion

#pragma region Event

		/// @brief Registra un tipo de evento.
		/// Los venetos deben registrarse as� para poder ser manejados
		/// por ECS.
		/// @tparam TEvent Tipo de evento.
		/// 
		/// @pre TEvent debe cumplir IsEcsEvent<TEvent>.
		template <typename TEvent> requires IsEcsEvent<TEvent>
		void RegisterEventType() {
			eventManager->RegisterEventType<TEvent>();
		}

		/// @brief Publica un evento, de tal manera que puede ser
		/// procesado por los sistemas consumidores.
		/// @tparam TEvent Tipo de evento.
		/// @param event Evento.
		/// 
		/// @pre TEvent debe cumplir IsEcsEvent<TEvent>.
		/// @pre TEvent debe haber sido registrado con RegisterEventType.
		/// 
		/// @throws EventNotRegisteredException Si el evento no ha sido previamente registrado.
		template <typename TEvent> requires IsEcsEvent<TEvent>
		void PublishEvent(const TEvent& event) {
			OSK_ASSERT(
				eventManager->EventHasBeenRegistered<TEvent>(),
				EventNotRegisteredException(TEvent::GetEventName()));

			eventManager->PublishEvent(event);
		}

		/// @brief Obtiene la cola con todos los eventos de un tipo
		/// que se han producido en un frame.
		/// @tparam TEvent Tipo de evento.
		/// @return Cola con todos los eventos. Puede ser una cola vac�a si
		/// no se ha producido ning�n evento en el frame.
		/// 
		/// @pre TEvent debe cumplir IsEcsEvent<TEvent>.
		/// @pre TEvent debe haber sido registrado con RegisterEventType.
		/// 
		/// @throws EventNotRegisteredException Si el evento no ha sido previamente registrado.
		template <typename TEvent> requires IsEcsEvent<TEvent>
		const DynamicArray<TEvent>& GetEventQueue() const {
			OSK_ASSERT(
				eventManager->EventHasBeenRegistered<TEvent>(),
				EventNotRegisteredException(TEvent::GetEventName()));

			return eventManager->GetEventQueue<TEvent>();
		}
		
		/// @tparam TEvent Tipo del evento.
		/// @return True si el evento fue registrado.
		template <typename TEvent> requires IsEcsEvent<TEvent>
		bool EventHasBeenRegistered() const {
			return eventManager->EventHasBeenRegistered<TEvent>();
		}

		/// @brief Vac�a todas las colas de eventos.
		/// Debe llamarse al terminar el frame.
		void _ClearEventQueues();

#pragma endregion


		/// @brief Crea un nuevo objeto vac�o y devuelve su ID.
		/// @return ID del nuevo objeto.
		/// 
		/// @warning ECS NO ES DUE�O DEL OBJETO, el juego es el encargado
		/// de eliminarlo mediante DestroyObject().
		GameObjectIndex OSKAPI_CALL SpawnObject();

		/// @brief Elimina un objeto.
		/// @param obj Referencia al ID.
		///
		/// @note Su ID cambia a 0 para indicar que ya no es una ID v�lida.
		void OSKAPI_CALL DestroyObject(GameObjectIndex* obj);

		/// @brief Finaliza el frame.
		/// Debe llamarse una vez al finalizar el frame.	
		void OSKAPI_CALL EndFrame();

		/// @brief Comprueba si un objeto est� activo.
		/// @param obj ID del objeto.
		/// @return True si est� activo, false si se elimin�.
		bool OSKAPI_CALL IsGameObjectAlive(GameObjectIndex obj) const;

		/// @brief Devuelve los render systems registrados.
		/// @return Render systems registrados.
		const OSKAPI_CALL DynamicArray<IRenderSystem*>& GetRenderSystems() const;

	private:

		template <typename TComponent> requires IsEcsComponent<TComponent>
		void EnsureRegistered() {
			if (!componentManager->ComponentHasBeenRegistered<TComponent>())
				RegisterComponent<TComponent>();
		}

		IO::ILogger* logger = nullptr;

		UniquePtr<SystemManager> systemManager;
		UniquePtr<ComponentManager> componentManager;
		UniquePtr<GameObjectManager> gameObjectManager;
		UniquePtr<EventManager> eventManager;

		DynamicArray<IRenderSystem*> renderSystems;

	};

}
