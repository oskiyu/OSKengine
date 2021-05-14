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

#include <functional>

namespace OSK {

	class GameObject;

	/// <summary>
	/// Representa la función new GameObject,
	/// que puede funcionar con cualquier subclase de GameObject.
	/// </summary>
	typedef std::function<GameObject*()> GameObjectCreateFunc;

	/// <summary>
	/// Clase que implementa un sistema ECS.
	/// Este sistema consiste en una serie de entidades (GameObject),
	/// que pueden tener componentes (Component),
	/// que son manejados por sistemas (System).
	/// </summary>
	class EntityComponentSystem {

	public:

		/// <summary>
		/// Destruye el ECS.
		/// Elimina todos los game objects.
		/// </summary>
		~EntityComponentSystem();

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
		/// Devuelve si un objeto tiene un componente en concreto.
		/// </summary>
		/// <typeparam name="T">Componente.</typeparam>
		/// <param name="object">Objeto.</param>
		/// <returns>True si tiene el componente.</returns>
		template <typename T> bool ObjectHasComponent(ECS::GameObjectID object) {
			return objectManager->GetSignature(object).test(componentManager->GetComponentType<T>());
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
			system->entityComponentSystem = this;
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
		/// Registra una clase derivada de GameObject.
		/// Para poder instanciarla.
		/// </summary>
		/// <typeparam name="T">Clase.</typeparam>
		/// <param name="className">Nombre de la clase.</param>
		template <typename T> void RegisterGameObjectClass(const std::string& className) {
			RegisterGameObjectClass(className, T::GetCreateFunction());
		}

		/// <summary>
		/// Registra una clase derivada de GameObject.
		/// Para poder instanciarla.
		/// </summary>
		/// <typeparam name="T">Clase.</typeparam>
		template <typename T> void RegisterGameObjectClass() {
			RegisterGameObjectClass<T>(T::GetClassName());
		}

		/// <summary>
		/// Registra una clase derivada de GameObject.
		/// Para poder instanciarla.
		/// </summary>
		/// <param name="className">Nombre de la clase.</param>
		/// <param name="func">Función que devuelve una nueva instancia de la clase.</param>
		void RegisterGameObjectClass(const std::string& className, GameObjectCreateFunc func);

		/// <summary>
		/// Spawnea un game object en el mundo.
		/// </summary>
		/// <param name="className">Nombre de la clase del game object.</param>
		/// <param name="position">Posición del objeto en el mundo.</param>
		/// <param name="axis">Eje sobre el que se va a aplicar la rotación inicial.</param>
		/// <param name="angle">Ángulo de la rotación inicial.</param>
		/// <param name="size">Escala inicial.</param>
		/// <returns>Puntero al nuevo objeto.</returns>
		GameObject* Spawn(const std::string& className, const Vector3f& position = { 0.0f }, const Vector3f& axis = { 0.0f, 1.0f, 0.0f }, float angle = 0.0f, const Vector3f& size = { 1.0f });

		/// <summary>
		/// Spawnea un game object en el mundo.
		/// </summary>
		/// <param name="className">Nombre de la clase del game object.</param>
		/// <param name="instanceName">Nombre de esta instancia.</param>
		/// <param name="position">Posición del objeto en el mundo.</param>
		/// <param name="axis">Eje sobre el que se va a aplicar la rotación inicial.</param>
		/// <param name="angle">Ángulo de la rotación inicial.</param>
		/// <param name="size">Escala inicial.</param>
		/// <returns>Puntero al nuevo objeto.</returns>
		GameObject* Spawn(const std::string& className, const std::string& instanceName, const Vector3f& position = { 0.0f }, const Vector3f& axis = { 0.0f, 1.0f, 0.0f }, float angle = 0.0f, const Vector3f& size = { 1.0f });

		/// <summary>
		/// Spawnea un game object en el mundo.
		/// </summary>
		/// <typeparam name="T">Clase del game object.</typeparam>
		/// <param name="position">Posición del objeto en el mundo.</param>
		/// <param name="axis">Eje sobre el que se va a aplicar la rotación inicial.</param>
		/// <param name="angle">Ángulo de la rotación inicial.</param>
		/// <param name="size">Escala inicial.</param>
		/// <returns>Puntero al nuevo objeto.</returns>
		template <typename T> T* Spawn(const Vector3f& position = { 0.0f }, const Vector3f& axis = { 0.0f, 1.0f, 0.0f }, float angle = 0.0f, const Vector3f& size = { 1.0f }) {
			return (T*)Spawn(T::GetClassName(), position, axis, angle, size);
		}

		/// <summary>
		/// Spawnea un game object en el mundo.
		/// </summary>
		/// <typeparam name="T">Clase del game object.</typeparam>
		/// <param name="instanceName">Nombre de la instancia.</param>
		/// <param name="position">Posición del objeto en el mundo.</param>
		/// <param name="axis">Eje sobre el que se va a aplicar la rotación inicial.</param>
		/// <param name="angle">Ángulo de la rotación inicial.</param>
		/// <param name="size">Escala inicial.</param>
		/// <returns>Puntero al nuevo objeto.</returns>
		template <typename T> T* Spawn(const std::string& instanceName, const Vector3f& position = { 0.0f }, const Vector3f& axis = { 0.0f, 1.0f, 0.0f }, float angle = 0.0f, const Vector3f& size = { 1.0f }) {
			return (T*)Spawn(T::GetClassName(), instanceName, position, axis, angle, size);
		}

		/// <summary>
		/// Devuelve el game object con la ID dada.
		/// </summary>
		/// <param name="id">ID.</param>
		/// <returns>Game object instance.</returns>
		GameObject* GetGameObjectByID(ECS::GameObjectID id) const;

		std::list<GameObject*>& GetAllGameObjects() {
			return gameObjectsReferences;
		}

	private:

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

		/// <summary>
		/// Map ID del objeto -> objeto.
		/// </summary>
		std::unordered_map<ECS::GameObjectID, GameObject*> gameObjects;

		/// <summary>
		/// Map nombre de objeto -> ID del objeto.
		/// </summary>
		std::unordered_map<std::string, ECS::GameObjectID> instancesByName;

		/// <summary>
		/// Map nombre de clase -> función crear clase.
		/// </summary>
		std::unordered_map<std::string, GameObjectCreateFunc> registeredGameObjectClasses;

		/// <summary>
		/// Referencias a los game objects spawneados.
		/// </summary>
		std::list<GameObject*> gameObjectsReferences;

	};

}

/// <summary>
/// Crea las funciones necesarias para poder registrar esta clase como un game object.
/// </summary>
/// <param name="x">Clase del objeto..</param>
#define OSK_GAME_OBJECT(x) \
	 static inline OSK::GameObjectCreateFunc GetCreateFunction() { return []() { return new x; }; } \
	 static inline std::string GetClassName() { return #x; } \
