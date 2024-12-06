#pragma once

#include "ApiCall.h"
#include "UniquePtr.hpp"

#include "IComponentController.h"
#include "IComponentView.h"

#include "ISystemController.h"
#include "ISystemView.h"

#include "GameObject.h"
#include "Component.h"

#include "HashMap.hpp"
#include <functional>
#include "OwnedPtr.h"
#include "NumericTypes.h"

#include <string>
#include <optional>

namespace OSK::ECS {
	class EntityComponentSystem;
}

namespace OSK::Editor {

	namespace UI {
		class EditorUi;
	}


	/// @brief Clase principal del editor.
	/// Maneja la interfaz de usuario y su conexión
	/// con el juego.
	class OSKAPI_CALL Editor {

	public:

		/// @brief Función que permite crear
		// un controlador para un tipo en concreto.
		using ComponentControllerFactoryMethod = std::function<OwnedPtr<Controllers::IComponentController>(ECS::GameObjectIndex, void*, Views::IComponentView*)>;

		/// @brief Función que permite crear
		// una vista para un tipo en concreto.
		using ComponentViewFactoryMethod = std::function<OwnedPtr<Views::IComponentView>(const Vector2f& size)>;


		/// @brief Función que permite crear
		// un controlador para un sistema en concreto.
		using SystemControllerFactoryMethod = std::function<OwnedPtr<Controllers::ISystemController>(ECS::ISystem*, Views::ISystemView*)>;

		/// @brief Función que permite crear
		// una vista para un sistema en concreto.
		using SystemViewFactoryMethod = std::function<OwnedPtr<Views::ISystemView>(const Vector2f& size)>;


		/// @brief Crea el editor.
		/// @param rootUiElement Elemento raíz de interfaz del juego.
		/// @param windowSize Tamaño de la ventana.
		/// @post Añade la interfaz de usuario del editor
		/// al elemento raíz de interfaz.
		Editor(
			OSK::UI::IContainer* rootUiElement,
			const Vector2f& windowSize);

#pragma region Component controller factory

		/// @brief Registra el método de creación
		/// de un tipo de controlador en concreto.
		/// @param componentName Nombre del componente
		/// con el que está relacionado el controlador.
		/// @param controllerFactoryMethod Método de creación.
		void RegisterComponentController(
			std::string_view componentName,
			const ComponentControllerFactoryMethod& controllerFactoryMethod);

		/// @brief Registra el método de creación.
		/// de un tipo de controlador en concreto.
		/// @tparam TComponentController Tipo del controlador.
		template <typename TComponentController>
			requires Controllers::IsComponentController<TComponentController>
		void RegisterComponentController() {
			RegisterComponentController(
				TComponentController::GetComponentTypeName(),
				[](ECS::GameObjectIndex obj, void* componentData, Views::IComponentView* view) {
					return new TComponentController(obj, componentData, view);
				});
		}

		/// @brief Registra el método de creación.
		/// @tparam TComponentController Tipo del controlador.
		/// @param factoryMethod Función de creación.
		template <typename TComponentController>
			requires Controllers::IsComponentController<TComponentController>
		void RegisterComponentController(const ComponentControllerFactoryMethod& factoryMethod) {
			RegisterComponentController(TComponentController::GetComponentTypeName(), factoryMethod);
		}

#pragma endregion

#pragma region Component view factory

		/// @brief Registra el método de creación
		/// de un tipo de vista en concreto.
		/// @param componentName Nombre del componente
		/// con el que está relacionado la vista.
		/// @param viewFactoryMethod Método de creación.
		void RegisterComponentView(
			std::string_view componentName,
			const ComponentViewFactoryMethod& viewFactoryMethod);

		/// @brief Registra el método de creación
		/// de un tipo de vista en concreto.
		/// @tparam TComponentView Tipo de la vista.
		template <typename TComponentView>
			requires Views::IsComponentView<TComponentView>
		void RegisterComponentView() {
			RegisterComponentView(TComponentView::GetComponentTypeName(), [](const Vector2f& size) { return new TComponentView(size); });
		}

		/// @brief Registra el método de creación
		/// de un tipo de vista en concreto.
		/// @tparam TComponentView Tipo de la vista.
		/// @param factoryMethod Método de creación.
		template <typename TComponentView>
			requires Views::IsComponentView<TComponentView>
		void RegisterComponentView(const ComponentViewFactoryMethod& factoryMethod) {
			RegisterComponentView(TComponentView::GetComponentTypeName(), factoryMethod);
		}

#pragma endregion

#pragma region System controller factory

		/// @brief Registra el método de creación
		/// de un tipo de controlador en concreto.
		/// @param componentName Nombre del componente
		/// con el que está relacionado el controlador.
		/// @param controllerFactoryMethod Método de creación.
		void RegisterSystemController(
			std::string_view componentName,
			const SystemControllerFactoryMethod& controllerFactoryMethod);

		/// @brief Registra el método de creación.
		/// de un tipo de controlador en concreto.
		/// @tparam TComponentController Tipo del controlador.
		template <typename TSystemController>
			requires Controllers::IsSystemController<TSystemController>
		void RegisterSystemController() {
			RegisterSystemController(
				TSystemController::GetSystemName(),
				[](ECS::ISystem* system, Views::ISystemView* view) {
					return new TSystemController(system, view);
				});
		}

		/// @brief Registra el método de creación.
		/// @tparam TComponentController Tipo del controlador.
		/// @param factoryMethod Función de creación.
		template <typename TSystemController>
			requires Controllers::IsSystemController<TSystemController>
		void RegisterSystemController(const SystemControllerFactoryMethod& factoryMethod) {
			RegisterSystemController(TSystemController::GetSystemName(), factoryMethod);
		}

#pragma endregion

#pragma region Component view factory

		/// @brief Registra el método de creación
		/// de un tipo de vista en concreto.
		/// @param componentName Nombre del componente
		/// con el que está relacionado la vista.
		/// @param viewFactoryMethod Método de creación.
		void RegisterSystemView(
			std::string_view systemName,
			const SystemViewFactoryMethod& viewFactoryMethod);

		/// @brief Registra el método de creación
		/// de un tipo de vista en concreto.
		/// @tparam TComponentView Tipo de la vista.
		template <typename TSystemView>
			requires Views::IsSystemView<TSystemView>
		void RegisterSystemView() {
			RegisterSystemView(TSystemView::GetSystemName(), [](const Vector2f& size) { return new TSystemView(size); });
		}

		/// @brief Registra el método de creación
		/// de un tipo de vista en concreto.
		/// @tparam TComponentView Tipo de la vista.
		/// @param factoryMethod Método de creación.
		template <typename TSystemView>
			requires Views::IsSystemView<TSystemView>
		void RegisterSystemView(const SystemViewFactoryMethod& factoryMethod) {
			RegisterSystemView(TSystemView::GetSystemName(), factoryMethod);
		}

#pragma endregion

		/// @brief Actualiza los controladores
		/// y las vistas activas.
		/// @param ecs Clase ECS del motor.
		/// @param deltaTime Tiempo transcurrido
		/// desde la última actualización.
		void Update(
			ECS::EntityComponentSystem* ecs,
			TDeltaTime deltaTime);


		/// @brief Establece el objeto seleccionado
		/// en el editor.
		/// @param gameObject ID del objeto
		/// seleccionado. Puede ser vacío
		/// para indicar que se ha deseleccionado.
		void SetSelectedObject(ECS::GameObjectIndex gameObject);

		/// @brief Establece que no haya ningún objeto
		/// seleccionado.
		void ClearSelectedObject();

		/// @brief Establece el sistema seleccionado
		/// en el editor.
		/// @param systemName Nombre del sistema seleccionado.
		/// Si está vacío, se considera que se ha deseleccionado
		/// el sistema anterior.
		void SetSelectedSystem(const std::string& systemName);

		/// @brief Establece que no haya ningún sistema
		/// seleccionado.
		void ClearSelectedSystem();

		/// @return Interfaz de usuario del editor.
		/// No es nullptr.
		UI::EditorUi* GetUi();

	private:

		struct ControllerViewPair {
			Controllers::IComponentController* controller;
			Views::IComponentView* view;
			bool isNewInsertion = false;
		};

		void UpdateByObject(ECS::EntityComponentSystem* ecs);
		void UpdateBySystem();

		/// @brief Devuelve el controlador y la vista
		/// para el componente indicado.
		/// Si no existe, lo crea previamente.
		/// @param ecs Clase ECS del motor.
		/// @param type Tipo de componente a buscar.
		/// @return Controlador para
		/// el componente indicado.
		/// Si el tipo de componente no tiene enlazado
		/// una pareja view-controller, devuelve vacío.
		std::optional<ControllerViewPair> GetOrCreateControllerViewPair(
			ECS::EntityComponentSystem* ecs,
			ECS::ComponentType type);

		std::unordered_map<std::string, ComponentControllerFactoryMethod, StringHasher, std::equal_to<>> m_componentControllersFactoryMethods;
		std::unordered_map<std::string, ComponentViewFactoryMethod, StringHasher, std::equal_to<>> m_componentViewsFactoryMethods;

		std::unordered_map<std::string, SystemControllerFactoryMethod, StringHasher, std::equal_to<>> m_systemControllerFactoryMethods;
		std::unordered_map<std::string, SystemViewFactoryMethod, StringHasher, std::equal_to<>> m_systemViewsFactoryMethods;


		std::unordered_map<ECS::ComponentType, UniquePtr<Controllers::IComponentController>> m_controllers;
		std::unordered_map<ECS::ComponentType, Views::IComponentView*> m_views;

		UniquePtr<Controllers::ISystemController> m_currentSystemController;

		// Estado.

		ECS::EntityComponentSystem* m_ecs = nullptr;

		ECS::GameObjectIndex m_selectedObject = ECS::GameObjectIndex::CreateEmpty();
		std::string m_selectedSystemName{};

		/// @brief Indica si el objeto seleccionado ha
		/// cambiado desde la última actualización.
		bool m_isSelectedObjectDirty = false;


		// UI & views.
		UI::EditorUi* m_editorUi;

	};

}
