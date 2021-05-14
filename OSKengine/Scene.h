#pragma once

#include "OSKmacros.h"

#include "ECS.h"
#include "ContentManager.h"
#include "GameObject.h"
#include "RenderizableScene.h"
#include "RenderSystem3D.h"

#include "Scanner.h"

#include <list>

namespace OSK {

	class Scene {

	public:

		/// <summary>
		/// Crea la escena.
		/// </summary>
		Scene(EntityComponentSystem* ecs, RenderSystem3D* renderSystem);

		/// <summary>
		/// Elimina la escena y todos sus game objects.
		/// </summary>
		~Scene();

		/// <summary>
		/// Carga la escena desde un archivo.
		/// </summary>
		/// <param name="path">Ruta del archivo (con extensión).</param>
		void Load(const std::string path);

		/// <summary>
		/// Carga la escena desde el código dado.
		/// </summary>
		void LoadScript(const std::string& code);

		/// <summary>
		/// Carga un skybox.
		/// </summary>
		/// <param name="path">Ruta del skybox (véase ContentManager).</param>
		void LoadSkybox(const std::string& path);

		/// <summary>
		/// Carga el terreno.
		/// </summary>
		/// <param name="path">Ruta del heightmap (con extensión).</param>
		/// <param name="quadSize">Tamaño de cada pixel del heightmap, en unidades del mundo.</param>
		/// <param name="maxHeight">Altura máxima del heightmap.</param>
		void LoadTerrain(const std::string& path, const Vector2f& quadSize, float maxHeight);

		/// <summary>
		/// Spawnea un game object en el mundo.
		/// Cuando se descargue la escena, se eliminará.
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
		/// Cuando se descargue la escena, se eliminará.
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
		/// Cuando se descargue la escena, se eliminará.
		/// </summary>
		/// <typeparam name="T">Clase del game object.</typeparam>
		/// <param name="position">Posición del objeto en el mundo.</param>
		/// <param name="axis">Eje sobre el que se va a aplicar la rotación inicial.</param>
		/// <param name="angle">Ángulo de la rotación inicial.</param>
		/// <param name="size">Escala inicial.</param>
		/// <returns>Puntero al nuevo objeto.</returns>
		template <typename T> T* Spawn(const Vector3f& position = { 0.0f }, const Vector3f& axis = { 0.0f, 1.0f, 0.0f }, float angle = 0.0f, const Vector3f& size = { 1.0f }) {
			auto ptr = entityComponentSystem->Spawn<T>(position, axis, angle, size);

			ptr->DeleteOnScene = [this](GameObject* obj) {
				objectsOnScene.remove(obj);

				objectsByInstanceName.erase(obj->GetInstanceName());
			};

			objectsOnScene.push_back(ptr);
			objectsByInstanceName[ptr->instanceName] = ptr;

			return ptr;
		}

		/// <summary>
		/// Spawnea un game object en el mundo.
		/// Cuando se descargue la escena, se eliminará.
		/// </summary>
		/// <typeparam name="T">Clase del game object.</typeparam>
		/// <param name="instanceName">Nombre de la instancia.</param>
		/// <param name="position">Posición del objeto en el mundo.</param>
		/// <param name="axis">Eje sobre el que se va a aplicar la rotación inicial.</param>
		/// <param name="angle">Ángulo de la rotación inicial.</param>
		/// <param name="size">Escala inicial.</param>
		/// <returns>Puntero al nuevo objeto.</returns>
		template <typename T> T* Spawn(const std::string& instanceName, const Vector3f& position = { 0.0f }, const Vector3f& axis = { 0.0f, 1.0f, 0.0f }, float angle = 0.0f, const Vector3f& size = { 1.0f }) {
			auto ptr = entityComponentSystem->Spawn<T>(instanceName, position, axis, angle, size);

			ptr->DeleteOnScene = [this](GameObject* obj) {
				objectsOnScene.remove(obj);

				objectsByInstanceName.erase(obj->GetInstanceName());
			};

			objectsOnScene.push_back(ptr);
			objectsByInstanceName[ptr->instanceName] = ptr;

			return ptr;
		}

		/// <summary>
		/// Devuelve el game object de la escena con el nombre dado.
		/// </summary>
		GameObject* GetGameObjectByName(const std::string& instanceName);

		/// <summary>
		/// Devuelve el game object de la escena con el nombre dado.
		/// </summary>
		template <typename T> T* GetGameObjectByName(const std::string& instanceName) {
			return (T*)GetGameObjectByName(instanceName);
		}

		/// <summary>
		/// Elimina todos los game objects de la escena.
		/// </summary>
		void Unload();

	private:

		/// <summary>
		/// Sistema ECS al que está ligada la escena.
		/// </summary>
		EntityComponentSystem* entityComponentSystem = nullptr;

		/// <summary>
		/// Sistema de renderizado que usa la escena.
		/// </summary>
		RenderSystem3D* renderSystem = nullptr;

		/// <summary>
		/// Referencias a los objetos de la escena.
		/// </summary>
		std::list<GameObject*> objectsOnScene;

		/// <summary>
		/// Map instance name -> game object.
		/// </summary>
		std::unordered_map<std::string, GameObject*> objectsByInstanceName;

		/// <summary>
		/// Objeto para cargar la escena.
		/// </summary>
		SceneSystem::Loader::Scanner sceneLoader;

	};

}
