#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "System.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "GameObjectManager.h"
#include "UniquePtr.hpp"

namespace OSK {

	class GameObject;

	/// <summary>
	/// Clase que implementa un sistema ECS.
	/// Este sistema consiste en una serie de entidades (GameObject),
	/// que pueden tener componentes (Component),
	/// que son manejados por sistemas (System).
	/// </summary>
	class EntityComponentSystem {

	public:

		/// <summary>
		/// Crea el ECS.
		/// </summary>
		EntityComponentSystem();

		/// <summary>
		/// Función OnTick().
		/// Ejecuta OnTick() en todos los sistemas.
		/// </summary>
		/// <param name="deltaTime">Delta.</param>
		void OnTick(deltaTime_t deltaTime);

		/// <summary>
		/// Función OnDraw().
		/// Ejecuta OnDraw() en el sistema de renderizado.
		/// </summary>
		/// <param name="cmdBuffer">Buffer de comandos de Vulkan.</param>
		/// <param name="i">Iteración.</param>
		void OnDraw(VkCommandBuffer cmdBuffer, uint32_t i);

		/// <summary>
		/// Crea un nuevo GameObject.
		/// </summary>
		/// <returns>ID del objeto.</returns>
		ECS::GameObjectID CreateGameObject();

		/// <summary>
		/// Destruye un GameObject y sus componentes.
		/// </summary>
		/// <param name="object">ID del objeto.</param>
		void DestroyGameObject(ECS::GameObjectID object);

		/// <summary>
		/// Registra un nuevo componente.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		template <typename T> void RegisterComponent() {
			componentManager->RegisterComponent<T>();
		}

		/// <summary>
		/// Añade un componente a un objeto.
		/// </summary>
		/// <typeparam name="C">Tipo de componente.</typeparam>
		/// <param name="object">ID del objeto.</param>
		/// <param name="component">Componente.</param>
		template <typename C> void AddComponent(ECS::GameObjectID object, C component) {
			static_assert(std::is_base_of<Component, C>::value, "ERROR: AddComponent: not a Component!");

			componentManager->AddComponent(object, component);
			GetComponent<C>(object).OnCreate();

			Signature signature = objectManager->GetSignature(object);
			signature.set(componentManager->GetComponentType<C>(), true);
			objectManager->SetSignature(object, signature);

			systemManager->GameObjectSignatureChanged(object, signature);
		}

		/// <summary>
		/// Quita un componente de un objeto.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		/// <param name="object">ID del objeto.</param>
		template <typename T> void RemoveComponent(ECS::GameObjectID object) {
			GetComponent<T>(object).OnRemove();
			componentManager->RemoveComponent<T>(object);

			Signature signature = objectManager->GetSignature(object);
			signature.set(componentManager->GetComponentType<T>(), false);
			objectManager->SetSignature(object, signature);

			systemManager->GameObjectSignatureChanged(object, signature);
		}

		/// <summary>
		/// Obtiene el componente de un objeto.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		/// <param name="object">ID del objeto.</param>
		/// <returns>Componente.</returns>
		template <typename T> T& GetComponent(ECS::GameObjectID object) {
			return componentManager->GetComponent<T>(object);
		}

		/// <summary>
		/// Devuelve la ID de un tipo de componente.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		/// <returns>ID.</returns>
		template <typename T> ComponentType GetComponentType() {
			return componentManager->GetComponentType<T>();
		}

		/// <summary>
		/// Registra y crea un nuevo sistema.
		/// </summary>
		/// <typeparam name="T">Tipo de sistema.</typeparam>
		/// <returns>Sistema creado.</returns>
		template <typename T> T* RegisterSystem() {
			T* system = systemManager->CreateSystem<T>();
			system->ECSsystem = this;
			systemManager->SetSignature<T>(system->GetSystemSignature());
			system->OnCreate();

			return system;
		}

		/// <summary>
		/// Establece el signature de un sistema.
		/// Signature: componentes que necesita el objeto para ser manejado por el sistema.
		/// </summary>
		/// <typeparam name="T">Tipo de sistema.</typeparam>
		/// <param name="signature">Componentes.</param>
		template <typename T> void SetSystemSignature(Signature signature) {
			systemManager->SetSignature<T>(signature);
		}
		
		/// <summary>
		/// Map ID del objeto -> objeto.
		/// </summary>
		std::unordered_map<ECS::GameObjectID, GameObject*> GameObjects;

	private:

		/// <summary>
		/// Maneja los sistemas.
		/// </summary>
		UniquePtr<ECS::SystemManager> systemManager;

		/// <summary>
		/// Maneja los componentes.
		/// </summary>
		UniquePtr<ECS::ComponentManager> componentManager;

		/// <summary>
		/// Maneja los objetos.
		/// </summary>
		UniquePtr<ECS::GameObjectManager> objectManager;

	};

}