#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "ComponentManager.h"
#include "GameObjectManager.h"
#include "SystemManager.h"
#include "ISystem.h"
#include "Assert.h"

namespace OSK::ECS {

	/// <summary>
	/// Entity-Component-System es un paradigma de programaci�n en el cual los objetos se dividen
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
	/// Toda operaci�n que el juego quiera hacer sobre el ECS debe hacerse a trav�s de
	/// esta clase.
	/// </summary>
	class OSKAPI_CALL EntityComponentSystem {

	public:

		EntityComponentSystem();

		/// <summary>
		/// Ejecuta la l�gica OnTick de todos los sistemas registrados.
		/// </summary>
		void OnTick(TDeltaTime deltaTime);

		/// <summary>
		/// Registra el componente.
		/// Todo componente que se quiera usar debe ser primero registrado.
		/// </summary>
		template <typename TComponent> void RegisterComponent() {
			componentManager->RegisterComponent<TComponent>();
		}

		/// <summary>
		/// A�ade el componente dado al objeto.
		/// </summary>
		/// <typeparam name="TComponent">Tipo de componente.</typeparam>
		/// <param name="obj">ID del objeto al que se va a a�adir..</param>
		/// <param name="component">Componente a a�adir.</param>
		template <typename TComponent> TComponent& AddComponent(GameObjectIndex obj, const TComponent& component) {
			OSK_ASSERT(!ObjectHasComponent<TComponent>(obj), "El objeto " + std::to_string(obj) + " ya tiene el componente " + TComponent::GetComponentTypeName() + ".");

			TComponent& oComponent = componentManager->AddComponent(obj, component);

			// Cambio de signature del objeto.
			Signature signature = gameObjectManager->GetSignature(obj);
			signature.SetTrue(componentManager->GetComponentType<TComponent>());
			gameObjectManager->SetSignature(obj, signature);

			systemManager->GameObjectSignatureChanged(obj, signature);

			return oComponent;
		}

		/// <summary>
		/// A�ade el componente dado al objeto.
		/// </summary>
		/// <typeparam name="TComponent">Tipo de componente.</typeparam>
		/// <param name="obj">ID del objeto al que se va a a�adir..</param>
		/// <param name="component">Componente a a�adir.</param>
		template <typename TComponent> TComponent& AddComponent(GameObjectIndex obj, TComponent&& component) {
			OSK_ASSERT(!ObjectHasComponent<TComponent>(obj), "El objeto " + std::to_string(obj) + " ya tiene el componente " + TComponent::GetComponentTypeName() + ".");

			auto& oComponent = componentManager->AddComponentMove<TComponent>(obj, std::move(component));

			// Cambio de signature del objeto.
			Signature signature = gameObjectManager->GetSignature(obj);
			signature.SetTrue(componentManager->GetComponentType<TComponent>());
			gameObjectManager->SetSignature(obj, signature);

			systemManager->GameObjectSignatureChanged(obj, signature);

			return oComponent;
		}

		/// <summary>
		/// Comprueba si el objeto dado tiene a�adido un componente del tipo dado.
		/// </summary>
		template <typename TComponent> bool ObjectHasComponent(GameObjectIndex obj) const {
			return gameObjectManager->GetSignature(obj).Get(
				componentManager->GetComponentType<TComponent>()
			);
		}

		/// <summary>
		/// Elimina el componente del objeto.
		/// </summary>
		template <typename TComponent> void RemoveComponent(GameObjectIndex obj) {
			OSK_ASSERT(ObjectHasComponent<TComponent>(obj), "El objeto " + std::to_string(obj) + " no tiene el componente " + TComponent::GetComponentTypeName() + ".");
			
			componentManager->RemoveComponent<TComponent>(obj);

			Signature signature = gameObjectManager->GetSignature(obj);
			signature.SetFalse(componentManager->GetComponentType<TComponent>());
			gameObjectManager->SetSignature(obj, signature);

			systemManager->GameObjectSignatureChanged(obj, signature);
		}

		/// <summary>
		/// Devuelve una referencia al componente del tipo dado del objeto.
		/// </summary>
		template <typename TComponent> TComponent& GetComponent(GameObjectIndex obj) const {
			OSK_ASSERT(ObjectHasComponent<TComponent>(obj), "El objeto " + std::to_string(obj) + " no tiene el componente " + TComponent::GetComponentTypeName() + ".");
			return componentManager->GetComponent<TComponent>(obj);
		}

		/// <summary>
		/// Devuelve el c�digo identificativo del componente dado.
		/// </summary>
		template <typename TComponent> ComponentType GetComponentType() const {
			return componentManager->GetComponentType<TComponent>();
		}

		/// <summary>
		/// Registra y almacena un nuevo sistema del tipo dado.
		/// </summary>
		template <typename TSystem> TSystem* RegisterSystem() {
			TSystem* output = systemManager->RegisterSystem<TSystem>();

			((ISystem*)output)->OnCreate();

			return output;
		}

		/// <summary>
		/// Devuelve la instancia almacenada del sistema del tipo dado.
		/// </summary>
		template <typename TSystem> TSystem* GetSystem() const {
			return systemManager->GetSystem<TSystem>();
		}

		/// <summary>
		/// Spawnea un nuevo objeto, y devuelve su ID.
		/// ECS NO ES DUE�O DEL OBJETO, el juego es el encargado
		/// de eliminarlo mediante DestroyObject().
		/// </summary>
		/// <returns></returns>
		GameObjectIndex SpawnObject();

		/// <summary>
		/// Elimina un objeto.
		/// Su ID cambia a 0 para indicar que ya no es una ID v�lida.
		/// </summary>
		void DestroyObject(GameObjectIndex* obj);

	private:

		UniquePtr<SystemManager> systemManager;
		UniquePtr<ComponentManager> componentManager;
		UniquePtr<GameObjectManager> gameObjectManager;

	};

}