#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ECS.h"
#include "Transform.h"

#include "Model.h"
#include "ContentManager.h"

namespace OSK {

	/// <summary>
	/// Objeto del juego.
	/// Representa una entidad del ECS.
	/// Puede implementar funcionalidad extra con components.
	/// </summary>
	class OSKAPI_CALL GameObject {

		friend class EntityComponentSystem;
		friend class Scene;

	public:

		OSK_GAME_OBJECT(GameObject)

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~GameObject() = default;

		/// <summary>
		/// Función que se ejecuta al spawnear el objeto.
		/// </summary>
		virtual void OnCreate() {}

		/// <summary>
		/// Función que se ejecuta al eliminarse el objeto.
		/// </summary>
		virtual void OnRemove() {}

		/// <summary>
		/// ID del objeto
		/// </summary>
		ECS::GameObjectID ID;

		/// <summary>
		/// Añade un componente al objeto.
		/// El objeto debe haber sido spawneado.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		/// <param name="component">Componente.</param>
		template <typename T> void AddComponent(T component) {
			entityComponentSystem->AddComponent<T>(ID, component);
		}

		/// <summary>
		/// Quita un componente del objeto.
		/// /// El objeto debe haber sido spawneado.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		template <typename T> void RemoveComponent() {
			entityComponentSystem->RemoveComponent<T>(ID);
		}

		/// <summary>
		/// Obtiene el componente del objeto.
		/// /// El objeto debe haber sido spawneado.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		/// <returns>Componente.</returns>
		template <typename T> T& GetComponent() {
			return entityComponentSystem->GetComponent<T>(ID);
		}

		/// <summary>
		/// Comprueba si este objeto tiene un componente del tipo dado.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		/// <returns>True si lo tiene.</returns>
		template <typename T> bool HasComponent() {
			return entityComponentSystem->ObjectHasComponent<T>(ID);
		}

		/// <summary>
		/// Elimina el objeto del mundo.
		/// Ejecuta OnRemove().
		/// </summary>
		void Remove();

		/// <summary>
		/// Añade un modelo al objeto.
		/// </summary>
		/// <param name="path">Ruta del modelo (con extensión).</param>
		/// <param name="content">Content manager que lo va a cargar.</param>
		void AddModel(const std::string& path, ContentManager* content);

		/// <summary>
		/// Añade un modelo animado al objeto.
		/// </summary>
		/// <param name="path">Ruta del modelo (con extensión).</param>
		/// <param name="content">Content manager que lo va a cargar.</param>
		void AddAnimatedModel(const std::string& path, ContentManager* content);

		/// <summary>
		/// Devuelve el nombre de la instancia.
		/// </summary>
		/// <returns>Nombre de la instancia.</returns>
		std::string GetInstanceName() const;

		/// <summary>
		/// Transform del objeto.
		/// </summary>
		Transform* GetTransform() {
			return &transform;
		}

	private:

		/// <summary>
		/// Transform del objeto.
		/// </summary>
		Transform transform;

		/// <summary>
		/// Sistema ECS.
		/// </summary>
		EntityComponentSystem* entityComponentSystem = nullptr;

		/// <summary>
		/// True si ha sido spawneado.
		/// </summary>
		bool hasBeenCreated = false;

		/// <summary>
		/// Función que elimina este puntero.
		/// </summary>
		std::function<void(GameObject* obj)> Delete;

		/// <summary>
		/// Función que elimina este puntero.
		/// </summary>
		std::function<void(GameObject* obj)> DeleteOnScene = [](GameObject* obj) {};

		/// <summary>
		/// Nombre de la instancia.
		/// </summary>
		std::string instanceName;

	};

}
