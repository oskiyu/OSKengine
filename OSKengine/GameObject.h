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
		/// Transform del objeto.
		/// </summary>
		Transform Transform3D{};

		/// <summary>
		/// Añade un componente al objeto.
		/// El objeto debe haber sido spawneado.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		/// <param name="component">Componente.</param>
		template <typename T> void AddComponent(T component) {
			ECSsystem->AddComponent<T>(ID, component);
		}

		/// <summary>
		/// Quita un componente del objeto.
		/// /// El objeto debe haber sido spawneado.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		template <typename T> void RemoveComponent() {
			ECSsystem->RemoveComponent<T>(ID);
		}

		/// <summary>
		/// Obtiene el componente del objeto.
		/// /// El objeto debe haber sido spawneado.
		/// </summary>
		/// <typeparam name="T">Tipo de componente.</typeparam>
		/// <returns>Componente.</returns>
		template <typename T> T& GetComponent() {
			return ECSsystem->GetComponent<T>(ID);
		}

		template <typename T> bool HasComponent() {
			return ECSsystem->ObjectHasComponent<T>(ID);
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

	private:

		/// <summary>
		/// Sistema ECS.
		/// </summary>
		EntityComponentSystem* ECSsystem = nullptr;

		/// <summary>
		/// True si ha sido spawneado.
		/// </summary>
		bool hasBeenCreated = false;

		/// <summary>
		/// Función que elimina este puntero.
		/// </summary>
		std::function<void(GameObject* obj)> Delete;

	};

}
