#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ECS.h"
#include "Transform.h"

namespace OSK {

	/// <summary>
	/// Objeto del juego.
	/// Representa una entidad del ECS.
	/// Puede implementar funcionalidad extra con components.
	/// </summary>
	class OSKAPI_CALL GameObject {

	public:

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

		/// <summary>
		/// Crea el objeto y lo coloca en el mundo.
		/// Ejecuta OnCreate().
		/// </summary>
		/// <param name="ecs">Sistema ECS que lo maneja.</param>
		/// <param name="position">Posición en el mundo.</param>
		/// <param name="axis">Eje sobre el que se va a aplicar la rotación.</param>
		/// <param name="angle">Ángulo de rotación.</param>
		/// <param name="size">Escala.</param>
		void Spawn(EntityComponentSystem* ecs, const Vector3f& position = { 0.0f }, const Vector3f& axis = { 0.0f, 1.0f, 0.0f }, float angle = 0.0f, const Vector3f& size = { 1.0f });

		/// <summary>
		/// Elimina el objeto del mundo.
		/// Ejecuta OnRemove().
		/// </summary>
		void Remove();

	private:

		/// <summary>
		/// Sistema ECS.
		/// </summary>
		EntityComponentSystem* ECSsystem = nullptr;

		/// <summary>
		/// True si ha sido spawneado.
		/// </summary>
		bool hasBeenCreated = false;

	};

}
